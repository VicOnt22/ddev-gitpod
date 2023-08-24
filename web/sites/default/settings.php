<?php
$settings['config_sync_directory'] = '../config/default';
$settings['file_temp_path'] = '../tmp';
$settings['file_private_path'] = '../private';
// #ddev-generated: Automatically generated Drupal settings file.
if (file_exists($app_root . '/' . $site_path . '/settings.ddev.php') && getenv('IS_DDEV_PROJECT') == 'true') {
  include $app_root . '/' . $site_path . '/settings.ddev.php';
}
if (file_exists($app_root . '/' . $site_path . '/settings.local.php')) {
   include $app_root . '/' . $site_path . '/settings.local.php';
}

$databases['default']['default'] = array (
  'database' => 'dbpod',
  'username' => 'root',
  'password' => '',
  'prefix' => '',
  'host' => 'localhost',
  'port' => '3306',
  'namespace' => 'Drupal\\mysql\\Driver\\Database\\mysql',
  'driver' => 'mysql',
  'autoload' => 'core/modules/mysql/src/Driver/Database/mysql/',
);
$settings['hash_salt'] = '5NRFKYMfxGVnBoBmDw7Jtby56ZXWe0ZnDF2HIeJVJuRsjkhkWZX-TgN4Z-KJQjWAvW8O-If2SA';
