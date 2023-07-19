<?php

/**
 * @file
 * Contains \Drupal\node_expire\Form\NodeExpireAdminSettings.
 */

namespace Drupal\node_expire\Form;

use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Render\Element;
use Drupal\Core\Link;


/**
 * Custom Settings Form extends ConfigFormBase
 */
class NodeExpireAdminSettings extends ConfigFormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'node_expire_admin_settings';
  }

  /**
   * {@inheritdoc}
   */
  protected function getEditableConfigNames() {
    return ['node_expire.settings'];
  }


  /**
   * @param array $form
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *
   * @return array
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    // Form constructor.
    $form = parent::buildForm($form, $form_state);
    // Default settings.
    $config = $this->config('node_expire.settings');
    $form['handle_content_expiry'] = [
      '#type' => 'fieldset',
      '#title' => t('Handle content expiry'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    ];
    $form['handle_content_expiry']['node_expire_handle_content_expiry'] = [
      '#type' => 'radios',
      '#title' => t('Handle content expiry'),
      '#default_value' => \Drupal::getContainer()->get('config.factory')->getEditable('node_expire.settings')->get('node_expire_handle_content_expiry'),
      '#options' => [
        0 => t('In legacy mode'),
        1 => t('Trigger "Content Expired" event every cron run when the node is expired'),
        2 => t('Trigger "Content Expired" event only once when the node is expired'),
      ],
      '#description' => t('In non-legacy mode node expiry is set for each node type separately and disabled by default.') . ' ' . t('Enable it at Structure -> Content types -> {Your content type} -> Edit -> Publishing options.') . '<br />' . t('"Trigger "Content Expired" event only once " option allows to ignore nodes, which already have been processed.') . '<br />' . t('Legacy mode means: not possible to allow expiry separately for each particular node type, trigger "Content Expired" event every cron run, legacy data saving'),
    ];

    // Visibility.
    $states = [
      'visible' => [
        ':input[name="node_expire_handle_content_expiry"]' => [
          [
            'value' => '1'
          ],
          ['value' => '2'],
        ]
      ]
    ];

    // Variable node_expire_date_entry_elements is not used in legacy mode,
    // so in legacy mode it is safe to keep any of its value.
    // It is necessary just to take care about proper validation
    // (see node_expire_admin_settings_validate below).
    $form['date_entry_elements'] = [
      '#type' => 'fieldset',
      '#title' => t('Date values entry elements'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#states' => $states,
    ];
    $form['date_entry_elements']['node_expire_date_entry_elements'] = [
      '#type' => 'radios',
      '#title' => t('Enter date values using'),
      '#default_value' => _node_expire_get_date_entry_elements(),
      '#options' => [
        0 => t('Text fields'),
        1 => t('Date popups'),
      ],
      '#description' => t('"Date popups" option requires Date module to be installed') . ' ' . t('with Date Popup enabled. This option is not available in legacy mode.'),
      '#states' => $states,
    ];

    $form['past_date_allowed'] = [
      '#type' => 'fieldset',
      '#title' => t('Expire date in the past'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    ];
    $form['past_date_allowed']['node_expire_past_date_allowed'] = [
      '#type' => 'checkbox',
      '#title' => t('Allow expire date in the past'),
      '#default_value' => \Drupal::getContainer()->get('config.factory')->getEditable('node_expire.settings')->get('node_expire_past_date_allowed'),
      '#description' => t('Checking this box will allow to save nodes with expire date in the past. This is helpful during site development and testing.'),
    ];

    // End of node_expire_admin_settings().
    return $form;
  }


  /**
   * @param array $form
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *
   * @return void
   */
  public function validateForm(array &$form, \Drupal\Core\Form\FormStateInterface $form_state) {
    if ($form_state->getValue(['node_expire_date_entry_elements']) == 1 && $form_state->getValue(['node_expire_handle_content_expiry']) != 0 && !\Drupal::moduleHandler()->moduleExists('datetime')) {
      $form_state->setErrorByName('date_entry_elements', t('To use "Date popups" option Datetime module should be installed from Core.'));
    }
    // End of node_expire_admin_settings_validate().
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {

    $config = $this->config('node_expire.settings');
    $config->set('node_expire_handle_content_expiry', $form_state->getValue('node_expire_handle_content_expiry'));
    $config->set('node_expire_date_entry_elements', $form_state->getValue('node_expire_date_entry_elements'));
    $config->set('node_expire_past_date_allowed', $form_state->getValue('node_expire_past_date_allowed'));
    $config->save();

//    foreach (Element::children($form) as $variable) {
//      $config->set($variable, $form_state->getValue($form[$variable]['#parents']));
//    }
//
//    if (method_exists($this, '_submitForm')) {
//      $this->_submitForm($form, $form_state);
//    }

     parent::submitForm($form, $form_state);
  }



}
