<?php

namespace Drupal\node_expire\Plugin\RulesAction;

use Drupal\rules\Core\RulesActionBase;

/**
 * Provides a 'Show message on your site' action.    // Unset node expired flag action
 *
 * @RulesAction(
 *   id = "node_expire_action_unset_expired",
 *   label = @Translation("NodeExpire Unset expired node"),
 *   category = @Translation("Content"),
 *   context_definitions = {
 *      "node" = @ContextDefinition("entity:node",
 *       label = @Translation("Node"),
 *       description = @Translation("Specifies the content item to Unset expired."),
 *       assignment_restriction = "selector"
 *     ),
 *   }
 * )
 *
 */

class NodeExpireActionUnsetExpired extends RulesActionBase {

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
    $node->lastnotify = 0;
    $node->expired = 0;
    $node->save();
    $stop=1;
//    $wrapper = entity_metadata_wrapper('node', $node);
//    $value = $wrapper->value();
//    $value->expired = 0;
//    $value->lastnotify = 0;
//    $wrapper->set($value);
//    $wrapper->save();

  }
}
