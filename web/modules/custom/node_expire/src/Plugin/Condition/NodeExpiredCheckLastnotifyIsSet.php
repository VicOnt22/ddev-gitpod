<?php

namespace Drupal\node_expire\Plugin\Condition;

use Drupal\node\NodeInterface;
use Drupal\rules\Core\RulesConditionBase;

/**
* Provides 'Node is Expired' condition.
*
*  @Condition(id = "node_expire_is_expired")
*
*/
class NodeExpiredCheckLastnotifyIsSet extends RulesConditionBase {

  /**
   * Checks if the node has the "Expired" flag on
   * and lastnotify is set
   *
   * @param \Drupal\node\NodeInterface $node
   *   The node to be checked.
   *
   * @return bool
   *   TRUE if node is expired
   *
   */
  public function doEvaluate(NodeInterface $node): bool {
    return (!empty($node->expire) && $node->expire <= time() && $node->expired == 1 && $node->lastnotify != 0);
  }

}
