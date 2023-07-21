<?php

namespace Drupal\node_expire\Plugin\RulesEvent;

use Drupal\Component\Plugin\Derivative\DeriverBase;
use Drupal\Core\Entity\ContentEntityTypeInterface;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\Plugin\Discovery\ContainerDeriverInterface;
use Drupal\Core\StringTranslation\StringTranslationTrait;
use Symfony\Component\DependencyInjection\ContainerInterface;

class EntityNodeExpireDeriver extends DeriverBase implements ContainerDeriverInterface {
  use StringTranslationTrait;
  /**
   * The entity type manager.
   *
   * @var \Drupal\Core\Entity\EntityTypeManagerInterface
   */
  protected $entityTypeManager;

  /**
   * Creates a new EntityNodeExpireDeriver object.
   *
   * @param \Drupal\Core\Entity\EntityTypeManagerInterface $entity_type_manager
   *   The entity type manager.
   */
  public function __construct(EntityTypeManagerInterface $entity_type_manager) {
    $this->entityTypeManager = $entity_type_manager;
  }
  public static function create(ContainerInterface $container, $base_plugin_id) {
    return new static($container->get('entity_type.manager'));
  }

  /**
   * {@inheritdoc}
   */
  public function getDerivativeDefinitions($base_plugin_definition) {
    foreach ($this->entityTypeManager->getDefinitions() as $entity_type_id => $entity_type) {
      // Only allow content entities and ignore configuration entities.
      if (!$entity_type instanceof ContentEntityTypeInterface) {
        continue;
      }
      // Only select node, avoiding entities like block, comment, user, etc.
      // TODO: remove if() below, to expire other entity types (will also need actions for them)
      if(($entity_type->getLabel()  == 'Content') && ($entity_type_id == 'node')) {
        $this->derivatives[$entity_type_id] = [
          'label' => $this->t('Node_Expire @entity_type', ['@entity_type' => $entity_type->getSingularLabel()]),
          'category' => $entity_type->getLabel(),
          'entity_type_id' => $entity_type_id,
          'context' => [
             $entity_type_id => [
               'type' => "entity:$entity_type_id",
               'label' => $entity_type->getLabel(),
             ],
          ],
       ] + $base_plugin_definition;
     }
   }

    return $this->derivatives;
  }

}
