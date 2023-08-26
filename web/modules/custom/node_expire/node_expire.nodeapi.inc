<?php

/**
 * @file
 * Node API integration.
 */
use Drupal\node\Entity;
use Drupal\node\NodeInterface;
use Drupal\Core\Datetime\DrupalDateTime;

/**
 * Implements hook_node_load().
 */
//function _node_expire_node_load($nodes, $types) {
function _node_expire_node_load($nodes) {
  // Only deal with node types that have the Node expire feature enabled.
  // @FIXME
  // Could not extract the default value because it is either indeterminate, or
  // not scalar. You'll need to provide a default value in
  // config/install/node_expire.settings.yml and config/schema/node_expire.schema.yml.
  $ntypes = \Drupal::getContainer()->get('config.factory')->getEditable('node_expire.settings')->get('node_expire_ntypes');
  $node_expire_enabled = [];

  // Check if node_expire are enabled for each node.
  // If node_expires are not enabled, do nothing.

  foreach ($nodes as $node) {
    // Store whether node_expires are enabled for this node.
    $nodeType=$node->getType();
    if (!empty($nodeType)) {
      if ((isset($ntypes[$nodeType])) and ($ntypes[$nodeType]['enabled'])) {
        $node_expire_enabled[] = $node->nid->value;
      }
    }
  }

// For nodes with node_expire enabled, fetch information from the database.
  if (!empty($node_expire_enabled)) {
    $handle_content_expiry = \Drupal::getContainer()->get('config.factory')->getEditable('node_expire.settings')->get('node_expire_handle_content_expiry');
    $result = \Drupal::database()->query(
      'SELECT n.nid, n.type, expire, expired, lastnotify
       FROM {node} n
         INNER JOIN {node_expire} ne
           ON n.nid = ne.nid
       WHERE n.nid
         IN (' . implode(",", array_map("intval", $node_expire_enabled)) . ')');

// https://stackoverflow.com/questions/4037145/mysql-how-to-select-rows-where-value-is-in-array

    foreach ($result as $record) {
      $stop=1;
      if ($handle_content_expiry == 0) {
        $nodes[$record->nid]->expire = $record->expire;
      }
      else {
        $ntype = isset($ntypes[$record->type]) ? $ntypes[$record->type] : NULL;
        $nodes[$record->nid]->expire = _node_expire_date_db_to_str($record->expire, $ntype);
      }
      $nodes[$record->nid]->expired = $record->expired;
      $nodes[$record->nid]->lastnotify = $record->lastnotify;
    }
  }
  $stop=1;
}

/**
 * Implements hook_node_prepare().
 */
function _node_expire_node_prepare(&$ntype, \Drupal\node\NodeInterface $node) {
  // To prevent default value 1969-12-31 check also $ntypes['default'].
$stop=1;
  if (!isset($node->expire) && $ntype['default']) {
    $node->expire = \Drupal::service('date.formatter')->format(intval(strtotime($ntype['default'])), 'custom', NODE_EXPIRE_FORMAT);
  }elseif (isset($node->expire) && $ntype['default'] && !\Drupal::currentUser()->hasPermission('edit node expire')) {
  // This gives a way to users without edit exipration permission to update nodes with default expiration.
    $node->expire = \Drupal::service('date.formatter')->format(intval(strtotime($ntype['default'])), 'custom', NODE_EXPIRE_FORMAT);
  }elseif (!isset($node->expire)){
    $node->expire = '';
  }

   // Make sure the 'expire' property is set on the node, which wouldn't happen
   // if one of the above conditions failed. it seems that is last elseif() already doing, so we commented below two lines

   //   if (!property_exists($node, 'expire')) {
   //     $node->expire = NULL;
   //   }

}

/**
 * Implements hook_node_validate().
 */
function _node_expire_node_validate(&$ntype, \Drupal\node\NodeInterface $node) {
  $handle_content_expiry = \Drupal::getContainer()->get('config.factory')->getEditable('node_expire.settings')->get('node_expire_handle_content_expiry');
  $stop=1;
  if ($handle_content_expiry != 0) {
    if (!isset($ntype['enabled']) || !$ntype['enabled']) {
      return;
    }
  }

  // The only restriction we have is that the node can't expire in the past.
  if ($node->expire == '') {
    if (!empty($ntype['required']) && $ntype['default']) {
      \Drupal::messenger()->addError(t('You must choose an expiration date.'));
    }
  }
  elseif (!$expire = strtotime($node->expire) or $expire <= 0) {
//    $form_state->setErrorByName('expire_date', t('You have to specify a valid expiration date.'));
    \Drupal::messenger()->addError(t('You have to specify a valid expiration date.'));
  }
  elseif (\Drupal::getContainer()->get('config.factory')->getEditable('node_expire.settings')->get('node_expire_past_date_allowed') == 0 && $expire <= \Drupal::time()->getRequestTime()) {
    \Drupal::messenger()->addError(t("You can't expire a node in the past!"));
  }
  elseif (!empty($ntype['max']) and $expire > strtotime(intval($ntype['max']), $node->getCreatedTime())) {
    \Drupal::messenger()->addError(t('It must expire before %date.',
    array(
      '%date' => \Drupal::service('date.formatter')->format(intval(strtotime($ntype['max']), $node->getCreatedTime()), 'custom', NODE_EXPIRE_FORMAT),
    )));
  }
}

/**
 * Implements hook_node_update() and hook_node_insert().
 */
function _node_expire_node_update_insert(&$ntype, \Drupal\node\NodeInterface $node) {
  $handle_content_expiry = \Drupal::getContainer()->get('config.factory')->getEditable('node_expire.settings')->get('node_expire_handle_content_expiry');
$stop=1;
  if ($handle_content_expiry == 0) {
    // Old (legacy) style of processing.
    // Has the expiration been removed, or does it exist?
    if (isset($node->expire)) {

      \Drupal::database()->delete('node_expire')
        ->condition('nid', $node->id())
        ->execute();
      // Should we create a new record?
      if ($node->expire) {
        if (\Drupal::service('date.formatter')->format(strtotime($node->expire))) {
          $node->expire = \Drupal::service('date.formatter')->format(strtotime($node->expire));
        }
        $node->expired = FALSE;
        if(isset($node->lastnotify)) {
          $lastnotify=$node->lastnotify;
        }else{
          $lastnotify = 0;
        }
        \Drupal::database()->insert('node_expire')->fields([
          'nid' => $node->id(),
          'expire' => $node->expire,
          'expired' => $node->expired,
          'lastnotify' => $lastnotify,
        ])->execute();
      }
    }
  }
  else {
    if (!isset($ntype['enabled']) || !$ntype['enabled']) {
      return;
    }

    // Create a proper $node_expire stdClass.
    $node_expire = new stdClass();
    $node_expire->nid = $node->id();

    // For compatibility with Node Clone module.
    // Set default $node->expire value if it is not set.
    if (!isset($node->expire)) {
      _node_expire_node_prepare($ntype, $node);
    }

    // Expire is set
    $date_expire = _node_expire_date_str_to_db($node->expire, $ntype);
    $node_expire->expire = $date_expire;

    // Lastnotify.
    if (isset($node->lastnotify)) {
      $node_expire->lastnotify = $node->lastnotify;
    }
    else {
      // Default value.
      $node_expire->lastnotify = 0;
    }

    // Expired.
    if (isset($node->expired)) {
      $node_expire->new_record = 0;
      $node_expire->expired = $node->expired;
      if ($node_expire->expire >= NODE_EXPIRE_NO_EXPIRE) {
        // No expiry for this node.
        $node_expire->expired = 0;
      }
    }
    elseif (isset($node->original->expired)) {
      // For VBO (Views Bulk Operations) compatibility.
      // With VBO it is necessary to get all Node expire information
      // from $node->original instead of $node.
      $node_expire->new_record = 0;
      $node_expire->expired = $node->original->expired;
      // Also get other Node expire values.
      // Expire.
      $date_expire = _node_expire_date_str_to_db($node->original->expire, $ntype);
      $node_expire->expire = $date_expire;  //HERE shall eb New VALUE when changed in node edit Form
      // Lastnotify.
      if (isset($node->original->lastnotify)) {
        $node_expire->lastnotify = $node->original->lastnotify;
      }
      else {
        // Default value.
        $node_expire->lastnotify = 0;
      }

      if ($node_expire->expire >= NODE_EXPIRE_NO_EXPIRE) {
        // No expiry for this node.
        $node_expire->expired = 0;
      }
    }
    else {
      // No record in the database yet.
      $node_expire->new_record = 1;
      // Default value.
      $node_expire->expired = 0;
    }

    // Write the record. $node_expire shall have new value
    _node_expire_write_record($node_expire, $node->id());
  }
}

/**
 * Writes node_expire record.
 */
function _node_expire_write_record($node_expire, $nid) {

  $intnid= (int) $nid;
  // Check, is it insert or update. // Vic - not needed as merge is doing checks automatically

//  $cnt = \Drupal::database()->query(
//    'SELECT count(nid)
//     FROM {node_expire} ne
//     WHERE ne.nid = :nid',
//    array(':nid' => $intnid))->fetchField();

  // Write the record.
//  if ($cnt == 0) {
//    // Insert.
//    \Drupal::database()->insert('node_expire')->fields([
//      'nid' => $intnid,
//      'expire' => $node_expire->expire,
//      'expired' => $node_expire->expired,
//      'lastnotify' => $node_expire->lastnotify,
//    ])->execute();
//  }
//  else {
//    // Update.
//    \Drupal::database()->merge('node_expire')->fields([
//      'nid' => $intnid,
//      'expire' => $node_expire->expire,
//      'expired' => $node_expire->expired,
//      'lastnotify' => $node_expire->lastnotify,
//    ])->key('nid')->execute();
//  }

  // merge automatcally execute "UPSERT" queries, a combination of UPDATE if 'key' exist and INSERT if key value not exist.
      \Drupal::database()->merge('node_expire')
        ->key('nid', $intnid)
        ->fields([
        'expire' => $node_expire->expire,
        'expired' => $node_expire->expired,
        'lastnotify' => $node_expire->lastnotify,
      ])->execute();

      $stop=1;
}

/**
 * Implements hook_node_delete().
 */
function _node_expire_node_delete(&$ntype, \Drupal\node\NodeInterface $node) {
  \Drupal::database()->delete('node_expire')
    ->condition('nid', $node->id())
    ->execute();
}


/**
 * @param $ntype
 * @param $ntypes
 * @param $form
 * @param $form_state
 * @param $form_id
 * @param \Drupal\node\NodeInterface $node
 *
 * @return void
 */
function _node_expire_form_alter_nodeform(&$ntype, &$ntypes, &$form, &$form_state, $form_id, \Drupal\node\NodeInterface $node) {
  // Check if the Node expire feature is enabled for the node type.

  $revision_ids = \Drupal::entityTypeManager()->getStorage('node')->revisionIds($node);
  $last_revision_id = end($revision_ids);
  $last_revision = \Drupal::entityTypeManager()->getStorage('node')->loadRevision($last_revision_id);
  $last_revision->setSyncing(False);

  $handle_content_expiry = \Drupal::getContainer()->get('config.factory')->getEditable('node_expire.settings')->get('node_expire_handle_content_expiry');

  if ($handle_content_expiry != 0) {
    if (empty($ntypes[$ntype]['enabled'])) {
      return;
    }
    // Replace not set to default string.
    if (!isset($node->expire)) {
      $ntype_expire = isset($ntypes[$ntype]) ? $ntypes[$ntype] : NULL; //vic to restore
      $node->expire = _node_expire_date_db_to_str('', $ntype_expire);

//      $last_revision->expire = _node_expire_date_db_to_str('', $ntype_expire);
      $stop=1;

    }

  }
  else {

    // Replace not set to empty string.
    if (!isset($node->expire)) {
      $node->expire = '';
    }
    // Convert the timestamp into a human-readable date - legacy branch.
    if (is_numeric($node->expire)) {
      $node->expire = \Drupal::service('date.formatter')->format(intval($node->expire), 'custom', NODE_EXPIRE_FORMAT);
      $stop=1;
    }

  }

  // This supports node to never expire.

  if(empty($ntypes[$ntype]['default']) && empty($node->expire)) {
    $ntypes[$ntype]['required'] = FALSE;
  }
  if (\Drupal::currentUser()->hasPermission('edit node expire')) {
    if (_node_expire_get_date_entry_elements()) {
      // Date popups.
      $expire_field = array(
        '#title' => t('Expiration date'),
        '#description' => t('Time date to consider the node expired. Format: %time (MM/DD/YYYY).',
          array(
            '%time' => \Drupal::service('date.formatter')->format(\Drupal::time()->getRequestTime(), 'custom', 'm/d/Y'),
          )
        ),
        '#type' => 'datetime',
        '#date_time_element' => 'none',
        '#date_format' => NODE_EXPIRE_FORMAT,
        '#required' => $ntypes[$ntype]['required'],
        '#default_value' => DrupalDateTime::createFromTimestamp(intval($node->expire)), //maybe set for 0 to appear current date(or not-expire/max available NODE_EXPIRE_NO_EXPIRE) instead of 01-01-1969
      );
    }
    else {
      // Text fields.
      $expire_field = array(
        '#title' => t('Expiration date'),
        '#description' => t('Time date to consider the node expired. Format: %time (YYYY-MM-DD).',
          array(
            '%time' => \Drupal::service('date.formatter')->format(\Drupal::time()->getRequestTime(), 'custom', NODE_EXPIRE_FORMAT),
          )
        ),
        '#type' => 'textfield',
        '#maxlength' => 25,
        '#required' => $ntypes[$ntype]['required'], //vic to restore
        '#default_value' => $node->expire,
      );
    }
  }
  //user does not have permission to edit node-expire field.
  else {
    $expire_field = array(
      '#type' => 'value',
      '#value' => intval($node->expire),
    );
  }

  // If we use hidden value, do not create fieldset.
   if ($expire_field['#type'] == 'value') {
      $form['options1'] = array();
      $form['options1']['expire'] = &$expire_field;
    }
  // If the form doesn't have the publishing options we'll create our own.
  elseif (empty($form['options']['#access'])) {
    $form['options1'] = array(
      '#type' => 'fieldset',
      '#title' => t('Node Expire publishing option'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#weight' => 95,
      '#group' => 'advanced',
    );
    $form['options1']['expire'] = &$expire_field;
    $stop=1;
  }
  else {
    $form['options1']['expire'] = &$expire_field;
  }

  if (isset($node->expired)) {
    $form['node_expire'] = array(
      '#type' => 'value',
      '#value' => TRUE,); //just as flag? A hidden value used in line 371 above (for $node->expired we have this value)
  }
  $stop=1;

}

/**
 * Convert date string to timestamp int.
 *
 * @param string $date_in
 *   String date representation for UI:
 *   not set, non-date string, date string.
 * @param object $ntype
 *   Node expire configuration for particular node type.
 *
 * @return int
 *   Timestamp (int) date representation for DB:
 *   timestamp int, NODE_EXPIRE_NO_EXPIRE as a special value.
 */
function _node_expire_date_str_to_db($date_in, $ntype) {
  if (!isset($date_in)) {
    $date_out = NODE_EXPIRE_NO_EXPIRE;
    return $date_out;
  }

  $date_inner = trim($date_in);
  if (!strtotime($date_inner)) {
    // Make a valid date string.
    if (isset($ntype) &&
      !empty($ntype->default) &&
      (strtotime($ntype->default))) {
      $date_inner = $ntype->default;
    }
    else {
      $date_out = NODE_EXPIRE_NO_EXPIRE;
      return $date_out;
    }
  }

  $date_out = strtotime($date_inner);

  return $date_out;
}

/**
 * Convert timestamp int to date string.
 *
 * @param int $date_in
 *   Timestamp (int) date representation for DB:
 *   not set, timestamp int.
 * @param array $ntype_expire
 *   Node expire configuration for particular node type.
 *
 * @return string
 *   String date representation for UI:
 *   date string or ''.
 */
function _node_expire_date_db_to_str($date_in, $ntype_expire) {
  $date_inner = $date_in;
  if ((empty($date_inner)) &&
    (!(_node_expire_is_valid_time_stamp($date_inner)))) {
    if (isset($ntype_expire) &&
      !empty($ntype_expire['default']) &&
      (strtotime($ntype_expire['default']))) {
      $date_out = $ntype_expire['default'];
    }
    else {
      $date_out = '';
    }
  }
  elseif ($date_inner >= NODE_EXPIRE_NO_EXPIRE) {
    $date_out = '';
  }
  else {
    $dt = new DateTime();
    $dt->setTimestamp($date_inner);
//    $date_out = date(NODE_EXPIRE_FORMAT, $date_inner);

    $date_out = $dt->format('Y-m-d');
  }
  return $date_out;
}

/**
 * Checks timestamp to be valid.
 */
function _node_expire_is_valid_time_stamp($timestamp) {

  $return_timestamp = ((string) (int) $timestamp === $timestamp)
    && ($timestamp <= PHP_INT_MAX)
    && ($timestamp >= ~PHP_INT_MAX);

  return $return_timestamp;
}
