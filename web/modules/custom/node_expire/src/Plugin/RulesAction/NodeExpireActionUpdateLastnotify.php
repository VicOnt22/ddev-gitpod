<?php

namespace Drupal\node_expire\Plugin\RulesAction;

use Drupal\rules\Core\RulesActionBase;

/**
 * Provides a 'Show message on your site' action.    // Unset node expired flag action
 *
 * @RulesAction(
 *   id = "node_expire_update_lastnotify",
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
    //

  }
}
