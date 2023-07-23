<?php

/**
 * @file
 * Rules module integration.
 */
//
///**
// * Implements hook_rules_action_info().  //D9 Write a <modulename>/src/Plugin/RulesAction/<actionname>.php class
// *                                        for each action that the module defines. Make this class
// *                                        extend \Drupal\rules\Core\RulesActionBase.
// *
// * @ingroup rules
// */
//function node_expire_rules_action_info() {
//  return array(
//    'node_expire_unset_expired' => array(
//      'arguments' => array(
//        'node' => array('type' => 'node', 'label' => t('content expired')),
//      ),
//      'label' => t('Unset the expired flag'),
//      'module' => 'node_expire',
//    ),
//
//    'node_expire_update_lastnotify' => array(
//      'arguments' => array(
//        'node' => array('type' => 'node','label' => t('content expired'),
//        ),
//      ),
//      'label' => t('Update lastnotify'),
//      'module' => 'node_expire',
//    ),
//
//  );
//}
//
///**
// * Implements hook_rules_condition_info().   // D9 Write a
// *                                              <modulename>/src/Plugin/Condition/<conditionname>.php
// *                                              class for each condition
// *
// * @ingroup rules
// */
//function node_expire_rules_condition_info() {
//  return array(
//    'node_expire_rules_expired_check' => array(
//      'arguments' => array(
//        'node' => array('type' => 'node', 'label' => t('Content')),
//      ),
//      'label' => t('Content is expired'),
//      'help' => 'Evaluates to TRUE, if the given content has one of the selected content types.',
//      'module' => 'node_expire',
//    ),
//    'node_expire_rules_expired_check_lastnotify' => array(
//      'arguments' => array(
//        'node' => array(
//          'type' => 'node',
//          'label' => t('Content'),
//        ),
//      ),
//      'label' => t('Content is expired: Check lastnotify'),
//      'help' => 'Evaluates to TRUE, if the given content is expired and lastnotified 2 weeks ago.',
//      'module' => 'node_expire',
//    ),
//    'node_expire_rules_expired_check_lastnotify_is_set' => array(
//      'arguments' => array(
//        'node'      => array('type' => 'node', 'label' => t('Content')),
//      ),
//      'label'     => t('Content is expired: Check lastnotify is set'),
//      'help'      => 'Evaluates to TRUE, if the given content is expired and lastnotified is set.',
//      'module' => 'node_expire',
//    ),
//  );
//}

///**
// * Implements hook_rules_event_info(). //replaced by Event Plugin
// *
// * @ingroup rules
// */
//function node_expire_rules_event_info() {
//  $events['node_expired'] = array(
//    'label' => t('content expired'),
//    'group' => t('Node'),
//    'variables' => array(
//      'node' => array(
//        'type' => 'node',
//        'label' => t('Node'),
//      ),
//    ),
//  );
//  return $events;
//}

///**
// * Checks if the node has the "Expired" flag on. // replaced by Condition Plugin
// *
// * @param object $node
// *   The Node object.
// */
//function node_expire_rules_expired_check($node) {
//  return (!empty($node->expire) && $node->expire <= \Drupal::time()->getRequestTime() && $node->expired == 1);
//}
//
///**
// * Checks lastnotify. // replaced by Condition Plugin
// *
// * Checks if the node has the "Expired" flag on
// * and lastnotify greater than or equal to 2 weeks.
// *
// * @param object $node
// *   The Node object.
// */
//function node_expire_rules_expired_check_lastnotify($node) {
//  return (!empty($node->expire) && $node->expire <= \Drupal::time()->getRequestTime() && $node->expired == 1 && $node->lastnotify <= (\Drupal::time()->getRequestTime() - (14 * 24 * 60 * 60)));
//}
//
///**
// * Checks lastnotify is set. // replaced by Condition Plugin
// *
// * Checks if the node has the "Expired" flag on
// * and lastnotify is set.
// *
// * @param object $node
// *   The Node object.
// */
//function node_expire_rules_expired_check_lastnotify_is_set($node) {
//  return (!empty($node->expire) && $node->expire <= time() && $node->expired == 1 && $node->lastnotify != 0);
//}

///**
// * Unsets the "Expired" flag on nodes. // Not clear if this is action? or just helper function
// *
// * @param object $node
// *   The Node object with all node IDs
// *   that should unset the expired flag.
// */
//function node_expire_unset_expired($node) {
//  $wrapper = entity_metadata_wrapper('node', $node);
//  $value = $wrapper->value();
//  $value->expired = 0;
//  $value->lastnotify = 0;
//  $wrapper->set($value);
//  $wrapper->save();
//}
//
///**
// * Updates lastnotify on nodes.  // Not clear if this is action? or just helper function - moved to action
// *
// * @param object $node
// *   The Node object with all node IDs
// *   that should update lastnotify.
// */
//function node_expire_update_lastnotify($node) {
//  $wrapper = entity_metadata_wrapper('node', $node);
//  $value = $wrapper->value();
//  $value->lastnotify = \Drupal::time()->getRequestTime();
//  $wrapper->set($value);
//  $wrapper->save();
//// replace D7 entity_metadata_wrapper() using D8 entity-api info at
////  https://www.drupal.org/docs/drupal-apis/entity-api/content-entity
//
//}
