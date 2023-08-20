<?php

namespace Drupal\node_expire\Plugin\RulesAction;

use Drupal\rules\Core\RulesActionBase;

/**
 * Provides a 'Show message on your site' action.    // Unset node expired flag action
 *
 * @RulesAction(
 *   id = "node_expire_action_update_lastnotify",
 *   label = @Translation("NodeExpire Update Lastnotify db column"),
 *   category = @Translation("Content"),
 *   context_definitions = {
 *      "node" = @ContextDefinition("entity:node",
 *       label = @Translation("Node"),
 *       description = @Translation("Specifies the content item to Update lastnotify."),
 *       assignment_restriction = "selector"
 *     ),
 *   }
 * )
 *
 */
class NodeExpireActionUpdateLastnotify extends RulesActionBase {

  /**
   * @param $node
   *
   * @return void
   */
  protected function doExecute($node) {
    // TODO: add checking if expire condition and updating DB?

    $nodeid = $node->nid;
    $nodeid2 = $node->id;
    $stop=1;

    $lastnotify_value = $node->lastnotify;

    $node->lastnotify = \Drupal::time()->getRequestTime();
    $node->save();
    $stop=1;

//    $wrapper = entity_metadata_wrapper('node', $node);
//    $value = $wrapper->value();
//    $value->lastnotify = \Drupal::time()->getRequestTime();
//    $wrapper->set($value);
//    $wrapper->save();
    // replace D7 entity_metadata_wrapper() using D8 entity-api info at
    //  https://www.drupal.org/docs/drupal-apis/entity-api/content-entity

  }
}
