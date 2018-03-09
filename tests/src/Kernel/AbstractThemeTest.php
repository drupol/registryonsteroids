<?php

namespace Drupal\Tests\registryonsteroids\Kernel;

use Drupal\Tests\registryonsteroids\AbstractTest;

/**
 * Class AbstractThemeTest.
 */
abstract class AbstractThemeTest extends AbstractTest {

  /**
   * {@inheritdoc}
   */
  protected function setUp() {
    parent::setUp();

    global $conf;

    $conf['theme_debug'] = FALSE;
    $conf['theme_default'] = 'ros_theme';
  }

}
