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
class NodeExpiredCheckLastnotify extends RulesConditionBase {

  /**
   * Checks if the node has the "Expired" flag on
   * and lastnotify greater than or equal to 2 weeks
   *
   * @param \Drupal\node\NodeInterface $node
   *   The node to be checked.
   *
   * @return bool
   *   TRUE if node is expired
   *
   */
  public function doEvaluate(NodeInterface $node): bool {
    if(isset($node->expire) && isset($node->expired) && isset($node->lastnotify)) {
      $ne = $node->expire;
      $ned = $node->expired;
      $nln = $node->lastnotify;
      $nev = $node->expire->value;
      $nedv = $node->expired->value;
      $nlnv = $node->lastnotify->value;
    }
    $stop=1;
    return (!empty($node->expire) && $node->expire->value <= \Drupal::time()->getRequestTime() && $node->expired->value == 1 && $node->lastnotify->value <= (\Drupal::time()->getRequestTime() - (14 * 24 * 60 * 60)));
  }

}
