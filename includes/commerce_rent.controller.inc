<?php

/**
 * EntityExampleBasicControllerInterface definition.
 *
 * We create an interface here because anyone could come along and
 * use hook_entity_info_alter() to change our controller class.
 * We want to let them know what methods our class needs in order
 * to function with the rest of the module, so here's a handy list.
 *
 * @see hook_entity_info_alter()
 */

interface EntityCommerceRentControllerInterface
  extends DrupalEntityControllerInterface {
  public function create();
  public function save($entity);
  public function delete($entity);
}

/**
 * EntityExampleBasicController extends DrupalDefaultEntityController.
 *
 * Our subclass of DrupalDefaultEntityController lets us add a few
 * important create, update, and delete methods.
 */
class EntityCommerceRentController
  extends DrupalDefaultEntityController
  implements EntityCommerceRentControllerInterface {

  /**
   * Create and return a new entity_example_basic entity.
   */
  public function create() {
    $entity = new stdClass();
    $entity->type = 'commerce_rent';
    $entity->basic_id = 0;
    $entity->bundle_type = 'first_example_bundle';
    $entity->item_description = '';
    return $entity;
  }

  /**
   * Saves the custom fields using drupal_write_record()
   */
  public function save($entity) {
    // If our entity has no basic_id, then we need to give it a
    // time of creation.
    if (empty($entity->basic_id)) {
      $entity->created = time();
    }
    // Invoke hook_entity_presave().
    module_invoke_all('entity_presave', $entity, 'commerce_rent');
    // The 'primary_keys' argument determines whether this will be an insert
    // or an update. So if the entity already has an ID, we'll specify
    // basic_id as the key.
    $primary_keys = $entity->basic_id ? 'basic_id' : array();
    // Write out the entity record.
    drupal_write_record('commerce_rent', $entity, $primary_keys);
    // We're going to invoke either hook_entity_update() or
    // hook_entity_insert(), depending on whether or not this is a
    // new entity. We'll just store the name of hook_entity_insert()
    // and change it if we need to.
    $invocation = 'entity_insert';
    // Now we need to either insert or update the fields which are
    // attached to this entity. We use the same primary_keys logic
    // to determine whether to update or insert, and which hook we
    // need to invoke.
    if (empty($primary_keys)) {
      field_attach_insert('commerce_rent', $entity);
    }
    else {
      field_attach_update('commerce_rent', $entity);
      $invocation = 'entity_update';
    }
    // Invoke either hook_entity_update() or hook_entity_insert().
    module_invoke_all($invocation, $entity, 'commerce_rent');
    return $entity;
  }

  /**
   * Delete a single entity.
   *
   * Really a convenience function for delete_multiple().
   */
  public function delete($entity) {
    $this->delete_multiple(array($entity));
  }

  /**
   * Delete one or more entity_example_basic entities.
   *
   * Deletion is unfortunately not supported in the base
   * DrupalDefaultEntityController class.
   *
   * @param $basic_ids
   *   An array of entity IDs or a single numeric ID.
   */
  public function delete_multiple($entities) {
    $basic_ids = array();
    if (!empty($entities)) {
      $transaction = db_transaction();
      try {
        foreach ($entities as $entity) {
          // Invoke hook_entity_delete().
          module_invoke_all('entity_delete', $entity, 'commerce_rent');
          field_attach_delete('commerce_rent', $entity);
          $basic_ids[] = $entity->basic_id;
        }
        db_delete('commerce_rent')
          ->condition('basic_id', $basic_ids, 'IN')
          ->execute();
      }
      catch (Exception $e) {
        $transaction->rollback();
        watchdog_exception('commerce_rent', $e);
        throw $e;
      }
    }
  }
}

/**
 * @} End of "defgroup entity_example".
 */
