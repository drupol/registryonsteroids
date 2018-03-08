# Registry On Steroïds

Registry On Steroïds(ROS) fixes a limitation in Drupal 7 core, where some preprocess/process hooks are not properly registered in the theme registry.

This module fixes this by detecting and registering all preprocess/process callbacks in the right order then rewriting the Drupal's theme registry.

By updating it, it provides an improved inheritance mechanism with the preprocess/process callbacks cascade.

# Details

When it comes to render a theme hook, through [a render array](https://www.drupal.org/docs/7/api/render-arrays/render-arrays-overview) or [the theme function](https://api.drupal.org/api/drupal/includes!theme.inc/function/theme/7.x), the Drupal's 7 default behavior is to run a set of callbacks for preprocess and a set of callback for process in [a particular order](https://api.drupal.org/api/drupal/includes!theme.inc/function/theme/7.x).

Example:

You're using the Bartik core theme and you want to render the theme hook `node` and add some variants like its bundle name and its view mode.

Instead of using `theme('node', [...]);`, you will use `theme('node__page__full', [...]);`.

Then, in your theme or module, you create a preprocess function: `[HOOK]_preprocess_node__page__full(&$variables, $hook);`.

When rendering your node, Drupal will run the preprocess callbacks in the following order:

* [template_preprocess()](https://api.drupal.org/api/drupal/includes%21theme.inc/function/template_preprocess/7.x)
* [template_preprocess_node()](https://api.drupal.org/api/drupal/modules%21node%21node.module/function/template_preprocess_node/7.x)
* [bartik_preprocess_node()](https://api.drupal.org/api/drupal/themes%21bartik%21template.php/function/bartik_preprocess_node/7.x)
* [HOOK]_preprocess_node__page__full()

Once those preprocess are executed, Drupal will try to render the theme hook.
The theme hook `node__page__full` doesn't exist per se, so Drupal will try to render its parent: `node__page`, but in our case, it doesn't exist either.
So Drupal will iterate until a valid theme hook is found, in this case: `node`.

So far so good.

But What happens if you want to apply the same preprocessing to all the node of type page regardless of the view mode ?

The first idea is to create a specific preprocess: `[HOOK]_preprocess_node__page(&$variables, $hook)`
Unfortunately, this preprocess will be completely ignored by Drupal.

This module fixes this behavior and let Drupal use intermediary or derivative preprocess/process callbacks.

An issue is open since on drupal.org to fix this behavior, see [#2563445](https://www.drupal.org/node/2563445).

This modules provides a configuration form where you can enable or disable the `theme_debug` option available [since Drupal 7.33](https://www.drupal.org/node/223440#theme-debug) and enable the rebuild of the registry at each page load.
             
# Submodules

**Registry On Steroïds Alter** updates(alter!) default Drupal's render arrays and extends their `#theme` and `#theme_wrappers` members.

Example:

When calling `theme('node', [...]);` to render a page node, ROS will alter the render array and in the end,
`theme('node__page__full', [...]);` will be used to render the page node.

This will allow themers and designers to use particular preprocess/process callbacks like the following in this order:

* `[HOOK]_preprocess_node(&$variables, $hook);`
* `[HOOK]_preprocess_node__page(&$variables, $hook);`
* `[HOOK]_preprocess_node__page__full(&$variables, $hook);`

# History

The code of this module comes from [Atomium](https://www.drupal.org/project/atomium), a Drupal 7 base theme that implements all of this in a theme.
The code of Atomium is inspired by the code of many themes, especially [Bootstrap](https://www.drupal.org/project/bootstrap).

The idea behind this module is to remove the code that alter the theme registry from the theme and make it available for anyone through a module so every theme can enjoy these enhancements.

# Issues to follow

This module is watching issues on drupal.org. Once these issues are fixed, we'll be able to update this module and hopefully deprecate it at a certain point.

* [#1119364](https://www.drupal.org/node/1119364)
* [#1545964](https://www.drupal.org/node/1545964)
* [#2563445](https://www.drupal.org/node/2563445)

Feel free to test patches from these issues and give your feedback so we can move them forward.

# Tests

To run the tests locally:

* `git clone https://github.com/drupol/registryonsteroids.git`
* `composer install`

Then if you want to modify the default settings of the Drupal installation, please copy the file `runner.yml.dist` into `runner.yml` and update that file according to your configuration.

* `./vendor/bin/run drupal:site-install`

At this point, the Drupal installation will be in the `build` directory.

To run the test properly, you must make sure that a web server is started on this directory.

Run this command in a new terminal: `cd build; ../vendor/bin/drush rs`

Then, you are able to run the tests:

* `./vendor/bin/grumphp run`

# Author

* [Pol Dellaiera](http://drupal.org/u/pol)

# Contributors

* [Mark Carver](https://www.drupal.org/u/markcarver)
* [Andreas Hennings](https://www.drupal.org/u/donquixote)
