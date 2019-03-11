<?php

namespace Drupal\registryonsteroids;

/**
 * Creates ThemeHookStub objects.
 */
final class ThemeHookStubFactory {

  /**
   * Functions indexed by hook name, phase and then weight.
   *
   * @var string[][][]
   *   Format: $[$hook][$phase_key][$weight] = $function
   */
  private $functionsByHookAndPhasekeyAndWeight;

  /**
   * The theme registry.
   *
   * @var array[]
   */
  private $registry;

  /**
   * Template functions indexed by phase and then weight.
   *
   * @var string[][]
   *   Format: $[$phase_key][$weight] = $function
   */
  private $templateFunctionsByPhasekeyAndWeight;

  /**
   * ThemeHookStubFactory constructor.
   *
   * @param array[] $registry
   *   The registry.
   * @param string[][][] $functions_by_hook_and_phasekey_and_weight
   *   The functions keyed by hook, phasekey and weight.
   */
  public function __construct(array $registry, array $functions_by_hook_and_phasekey_and_weight) {
    $this->registry = $registry;
    $this->functionsByHookAndPhasekeyAndWeight = $functions_by_hook_and_phasekey_and_weight;
    $this->templateFunctionsByPhasekeyAndWeight = $functions_by_hook_and_phasekey_and_weight['*'];
  }

  /**
   * Create a theme hook stub.
   *
   * @param string $hook
   *   The hook.
   * @param \Drupal\registryonsteroids\ThemeHookStub|null $parent
   *   The parent hook.
   *
   * @return \Drupal\registryonsteroids\ThemeHookStub|null
   *   A hook stub or null.
   */
  public function createStub($hook, ThemeHookStub $parent = NULL) {
    $info = isset($this->registry[$hook])
      ? $this->registry[$hook]
      : NULL;

    $functions_by_phasekey_and_weight = isset($this->functionsByHookAndPhasekeyAndWeight[$hook])
      ? $this->functionsByHookAndPhasekeyAndWeight[$hook]
      : array();

    if ($parent !== NULL) {
      return $parent->addVariant(
        $hook,
        $info,
        $functions_by_phasekey_and_weight);
    }

    if ($info === NULL) {
      return NULL;
    }

    return new ThemeHookStub(
      $hook,
      $info,
      $functions_by_phasekey_and_weight,
      isset($info['template'])
        ? $this->templateFunctionsByPhasekeyAndWeight
        : array());
  }

}
