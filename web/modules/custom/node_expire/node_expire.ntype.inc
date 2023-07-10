<?php

/**
 * @file
 * Node type integration.
 */
use Drupal\Core\Form\FormStateInterface;
use Symfony\Component\HttpFoundation\Request;
/**
 * Implements hook_form_alter().
 */
function _node_expire_form_node_type_form_alter_validate(&$form, &$form_state) {
  $node_expire = &$form_state->getValue('node_expire');
  if (!empty($node_expire) and !strtotime($node_expire)) {
    $form_state->setErrorByName('node_expire',
      t('This values should be in PHP <a href="https://www.php.net/manual/en/function.strtotime.php" target="_blank">strtotime format</a>.'));
  }

  $node_expire_max = &$form_state->getValue('node_expire_max');
  if (!empty($node_expire_max)) {
    if (!strtotime($node_expire_max)) {
      $form_state->setErrorByName('node_expire_max',
        t('This values should be in PHP <a href="https://www.php.net/manual/en/function.strtotime.php" target="_blank">strtotime format</a>.'));
    }
    elseif (strtotime($node_expire) > strtotime($node_expire_max)) {
      $form_state->setErrorByName('node_expire_max',
        t('This value cannot be earlier then the default value.'));
    }
  }

  $node_expire_required = &$form_state->getValue('node_expire_required');
  if (!empty($node_expire_required)) {
    if (empty($node_expire)) {
      $form_state->setErrorByName('node_expire',
        t('Default expiration date should be set with expiration date required.'));
    }
  }
}

/**
 * Implements hook_form_alter().
 */
//function _node_expire_form_node_type_form_alter_submit(&$form, FormStateInterface &$form_state, $form_id) {
function _node_expire_form_node_type_form_alter_submit(&$form, FormStateInterface &$form_state) {
  // @FIXME
// Could not extract the default value because it is either indeterminate, or
// not scalar. You'll need to provide a default value in
// config/install/node_expire.settings.yml and config/schema/node_expire.schema.yml.
$ntypes = \Drupal::getContainer()->get('config.factory')->getEditable('node_expire.settings')->get('node_expire_ntypes');
$stop=1;
  $handle_content_expiry = \Drupal::getContainer()->get('config.factory')->getEditable('node_expire.settings')->get('node_expire_handle_content_expiry');
  if ($handle_content_expiry != 0) {
    $ntypes[$form_state->getValue('type')]['enabled']   = $form_state->getValue('node_expire_enabled');
  }

  $ntypes[$form_state->getValue('type')]['default']   = $form_state->getValue('node_expire');
  $ntypes[$form_state->getValue('type')]['max']       = $form_state->getValue('node_expire_max');
  $ntypes[$form_state->getValue('type')]['required']  = $form_state->getValue('node_expire_required');

  \Drupal::getContainer()->get('config.factory')->getEditable('node_expire.settings')->set('node_expire_ntypes', $ntypes)->save();
}
