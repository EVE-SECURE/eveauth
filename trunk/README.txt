// $Id$

Eveauth - Eve online authentication layer for Drupal
----------------------------------------------------

To install, place the entire Eveauth folder into your modules directory. Go to Administer -> Site building -> Modules and enable the module.

Now go to Administer -> Site configuration -> Eveauth. Get your user id and limited API key from http://www.eve-online.com/api/default.asp and paste them into the user id and api key field.

Select if you are in a corporation or alliance, select if you want persistent API check enabled and then click "Save configuration".

Next select the character which contains the corporation and/or alliance information used for authenticating. Click "Save configuration" again.

The next and final step involves clicking on the "Roles setup" and create the apropriate roles by hooking the checkbox and clicking "Save configuration" again.

Eveauth should now be up and running.

The corporation/alliance which this api key is attached to will be used for authenticating new users registering at the site. Therefore be sure to provide the correct api and select the correct character from the account the api belongs to. Users will be able to register even if they do not belong in your corporation or alliance, but they will not get corporation roles, prohibiting them and effectively blocking them from elevated access. In this way you can better fine tune your permission system. It is also usefull for checking the characters of would be employees before you accept their application by viewing their character sheet from the user accounts profile page (user/[userid]/characters).

Known incompatibilities
-----------------------

Eveauth is fresh from the oven. No incompatibilities with other modules are known to this date.

Maintainers
-----------
Eveauth is maintained and originaly developed by Helge Nordgård / aka: 'Helgur' in game. Developing, supporting and maintaining such a module requires a lot of effort, so in game donations is always welcome with appriciation.

