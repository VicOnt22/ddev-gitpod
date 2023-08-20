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
class NodeExpiredCheck extends RulesConditionBase {

  /**
   * Determines whether Node is Expired.
   *
   * @param \Drupal\node\NodeInterface $node
   *   The node to be checked.
   *
   * @return bool
   *   TRUE if node is expired
   *
   */
  public function doEvaluate(NodeInterface $node): bool {
    if(isset($node->expire) && isset($node->expired)) {
      $ne=$node->expire;
      $ned=$node->expired;
      $nev = $node->expire->value;
      $nedv = $node->expired->value;
    }
    $stop=1;
    return (!empty($node->expire) && ($node->expire->value <= \Drupal::time()->getRequestTime()) && $node->expired->value == 1);
  }

}
