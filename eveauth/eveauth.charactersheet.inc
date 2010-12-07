<?php
// $Id$
/**
 * Retrieve and show the accounts main and alternative characters as
 * a character sheet with skills.
 *
 * @file
 */

function user_character_sheets($user) {
    $markup = array ();

    // Retrieve a list of available characters from the api servers
    $user_api   = db_result(db_query('SELECT api FROM eveauth WHERE drupalid="'.$user->uid.'"', 2));
    $user_id    = db_result(db_query('SELECT userid FROM eveauth WHERE drupalid="'.$user->uid.'"', 2));

    if (!$user_api) {
        return drupal_set_message("The specified user does not have a API key associated with it. Character sheet can not be displayed with this user", 'error', FALSE);
    }

    $api_url = 'http://api.eve-online.com/account/Characters.xml.aspx?apiKey='.$user_api.'&userID='.$user_id;
    $http_result = drupal_http_request($api_url);
    $doc = simplexml_load_string($http_result->data);

    $count_items = count($doc->result->rowset->row);

    // Retrieve all of the character ID's from api and store them in
    // an array. These values will be needed to get the character sheets

    $player_id = array ();
    for ($i = 0; $i < $count_items; $i++) {
       $player_id[$i] = $doc->result->rowset->row[$i]->attributes()->characterID;
    }

    $character_sheet = array ();



    // Start main iteration which will get the sheets for all the characters
    // and print them embedded with html. Each of the characters will be given
    // its own post in array $html; which will be iterated

    foreach ($player_id as $key => $character) {
        $character_url = 'http://api.eve-online.com/char/CharacterSheet.xml.aspx?apiKey='.$user_api.'&characterID='.$character.'&userID='.$user_id;
        $http_result = drupal_http_request($character_url);
        $doc = simplexml_load_string($http_result->data);

        // Retrieve active skill in training queue for this character

        $skilltraining_url = 'http://api.eve-online.com/char/SkillInTraining.xml.aspx?apiKey='.$user_api.'&characterID='.$character.'&userID='.$user_id;
        $http_result2 = drupal_http_request($skilltraining_url);
        $training = simplexml_load_string($http_result2->data);

        $skill      = db_result(db_query('SELECT name FROM eveauth_skills WHERE skillid="'.$training->result->trainingTypeID.'"', 2));


        // Add Drupal collapse ajax snippet for skill categories
        drupal_add_js('misc/collapse.js');

        // Output of general data for character:
        // Race, Bloodline, Gender, Active corporation, Current skill in queue,
        // clone skillpoint threshold, clone type and wallet balance
        //

        $count_skills = count($doc->result->rowset->row);
        for($i = 0; $i < $count_skills; $i++) {
             $total_skillpoints = $total_skillpoints + $doc->result->rowset->row[$i]->attributes()->skillpoints;
        }

        $html[$key] = "<img src=\"http://img.eve.is/serv.asp?s=256&c=".$character."\" style=\"float: left; margin-right: 8px;\">
                <h2>".t('Character Sheet for '.$doc->result->name)."</h2>
                <strong>".t('Race:')."</strong>
                  ".
                          $doc->result->race."<br />
                <strong>".t('Bloodline:')."</strong>
                  ".
                          $doc->result->bloodLine."<br />
                <strong>".t('Gender:')."</strong>
                  ".
                          $doc->result->gender."<br />
                <strong>".t('Place of employment:')."</strong>
                  ".
                          $doc->result->corporationName."<br /><hr />
                <strong>".t('Currently training:')."</strong>
                  ".
                          $skill." to <strong>level ".$training->result->trainingToLevel."</strong> (Training will end on ".$training->result->trainingEndTime.")<br />
                <strong>".t('Skill points:')."</strong>
                  ".
                          $total_skillpoints."<br />
                <strong>".t('Clone status:')."</strong>
                  ".

                          $doc->result->cloneName."<br />
                <strong>".t('Monetary assets:')."</strong>
                  ".
                          convertcash($doc->result->balance)."<br /><br />
                <h3>Character Skill sheet</h3>";


                // Begin pulling trained skills for active character and match
                // them to our skills in DB so we can identify them with a
                // human understandable string/name

                $skilltree = array ();
                $sql = "SELECT name, groupid FROM {eveauth_skillgroups}";
                $results = db_query($sql);
                while($fields = db_fetch_array($results)) {
                    $skilltree[$fields["groupid"]] = $fields["name"];
                }

                $count_items = count($doc->result->rowset[0]->row);
                $selectedskill = array ();
                $character_skills = array ();
                for ($i = 0; $i < $count_items; $i++) {
                    $sql      = ('SELECT name,groupid FROM {eveauth_skills} WHERE skillid="'.$doc->result->rowset[0]->row[$i]->attributes()->typeID.'"');
                    $skillref = db_query($sql);
                    $fields = db_fetch_array($skillref);
                    $character_skills[$i]["name"]           = $fields["name"];
                    $character_skills[$i]["skillpoints"]    = $doc->result->rowset[0]->row[$i]->attributes()->skillpoints;
                    $character_skills[$i]["level"]          = $doc->result->rowset[0]->row[$i]->attributes()->level;
                    $character_skills[$i]["groupid"]        = $fields["groupid"];
                 }

                 // Prepare array for each of the skill category

                 $header = array(t('Skill name'), t('Amounted SP for skill'), t('Current level'));
                 $rows_corporation_management       = array ();
                 $rows_drones                       = array ();
                 $rows_electronics                  = array ();
                 $rows_engineering                  = array ();
                 $rows_gunnery                      = array ();
                 $rows_industry                     = array ();
                 $rows_leadership                   = array ();
                 $rows_missile_launcher_operation   = array ();
                 $rows_learning                     = array ();
                 $rows_mechanic                     = array ();
                 $rows_navigation                   = array ();
                 $rows_science                      = array ();
                 $rows_social                       = array ();
                 $rows_spaceship_command            = array ();
                 $rows_subsystems                   = array ();
                 $rows_trade                        = array ();
                 $rows_pi                           = array ();


                 // Begin sorting skills in the array / category they belong

                 for ($i = 0; $i < $count_items; $i++) {
                     if ($character_skills[$i]["groupid"] == "266") {
                         $rows_corporation_management[] = array (
                             $character_skills[$i]["name"],
                             $character_skills[$i]["skillpoints"],
                             $character_skills[$i]["level"],
                         );
                     }

                 }

                 for ($i = 0; $i < $count_items; $i++) {
                     if ($character_skills[$i]["groupid"] == "273") {
                         $rows_drones[] = array (
                             $character_skills[$i]["name"],
                             $character_skills[$i]["skillpoints"],
                             $character_skills[$i]["level"],
                         );
                     }

                 }

                 for ($i = 0; $i < $count_items; $i++) {
                     if ($character_skills[$i]["groupid"] == "272") {
                         $rows_electronics[] = array (
                             $character_skills[$i]["name"],
                             $character_skills[$i]["skillpoints"],
                             $character_skills[$i]["level"],
                         );
                     }

                 }

                 for ($i = 0; $i < $count_items; $i++) {
                     if ($character_skills[$i]["groupid"] == "271") {
                         $rows_engineering[] = array (
                             $character_skills[$i]["name"],
                             $character_skills[$i]["skillpoints"],
                             $character_skills[$i]["level"],
                         );
                     }

                 }

                  for ($i = 0; $i < $count_items; $i++) {
                     if ($character_skills[$i]["groupid"] == "255") {
                         $rows_gunnery[] = array (
                             $character_skills[$i]["name"],
                             $character_skills[$i]["skillpoints"],
                             $character_skills[$i]["level"],
                         );
                     }

                 }

                  for ($i = 0; $i < $count_items; $i++) {
                     if ($character_skills[$i]["groupid"] == "268") {
                         $rows_industry[] = array (
                             $character_skills[$i]["name"],
                             $character_skills[$i]["skillpoints"],
                             $character_skills[$i]["level"],
                         );
                     }

                 }

                 for ($i = 0; $i < $count_items; $i++) {
                     if ($character_skills[$i]["groupid"] == "258") {
                         $rows_leadership[] = array (
                             $character_skills[$i]["name"],
                             $character_skills[$i]["skillpoints"],
                             $character_skills[$i]["level"],
                         );
                     }

                 }

                 for ($i = 0; $i < $count_items; $i++) {
                     if ($character_skills[$i]["groupid"] == "256") {
                         $rows_missile_launcher_operation[] = array (
                             $character_skills[$i]["name"],
                             $character_skills[$i]["skillpoints"],
                             $character_skills[$i]["level"],
                         );
                     }

                 }

                  for ($i = 0; $i < $count_items; $i++) {
                     if ($character_skills[$i]["groupid"] == "267") {
                         $rows_learning[] = array (
                             $character_skills[$i]["name"],
                             $character_skills[$i]["skillpoints"],
                             $character_skills[$i]["level"],
                         );
                     }

                 }

                   for ($i = 0; $i < $count_items; $i++) {
                     if ($character_skills[$i]["groupid"] == "269") {
                         $rows_mechanic[] = array (
                             $character_skills[$i]["name"],
                             $character_skills[$i]["skillpoints"],
                             $character_skills[$i]["level"],
                         );
                     }

                 }

                  for ($i = 0; $i < $count_items; $i++) {
                     if ($character_skills[$i]["groupid"] == "275") {
                         $rows_navigation[] = array (
                             $character_skills[$i]["name"],
                             $character_skills[$i]["skillpoints"],
                             $character_skills[$i]["level"],
                         );
                     }

                 }

                  for ($i = 0; $i < $count_items; $i++) {
                     if ($character_skills[$i]["groupid"] == "270") {
                         $rows_science[] = array (
                             $character_skills[$i]["name"],
                             $character_skills[$i]["skillpoints"],
                             $character_skills[$i]["level"],
                         );
                     }

                 }

                 for ($i = 0; $i < $count_items; $i++) {
                     if ($character_skills[$i]["groupid"] == "278") {
                         $rows_social[] = array (
                             $character_skills[$i]["name"],
                             $character_skills[$i]["skillpoints"],
                             $character_skills[$i]["level"],
                         );
                     }

                 }

                 for ($i = 0; $i < $count_items; $i++) {
                     if ($character_skills[$i]["groupid"] == "257") {
                         $rows_spaceship_command[] = array (
                             $character_skills[$i]["name"],
                             $character_skills[$i]["skillpoints"],
                             $character_skills[$i]["level"],
                         );
                     }

                 }

                  for ($i = 0; $i < $count_items; $i++) {
                     if ($character_skills[$i]["groupid"] == "989") {
                         $rows_subsystems[] = array (
                             $character_skills[$i]["name"],
                             $character_skills[$i]["skillpoints"],
                             $character_skills[$i]["level"],
                         );
                     }

                 }

                  for ($i = 0; $i < $count_items; $i++) {
                     if ($character_skills[$i]["groupid"] == "274") {
                         $rows_trade[] = array (
                             $character_skills[$i]["name"],
                             $character_skills[$i]["skillpoints"],
                             $character_skills[$i]["level"],
                         );
                     }

                 }

                  for ($i = 0; $i < $count_items; $i++) {
                     if ($character_skills[$i]["groupid"] == "1044") {
                         $rows_pi[] = array (
                             $character_skills[$i]["name"],
                             $character_skills[$i]["skillpoints"],
                             $character_skills[$i]["level"],
                         );
                     }

                 }

                // Embed all of the skill array/categories with html

                 $output_of_skill_sheet = "
                     <fieldset class=\"collapsible collapsed\">
                      <legend>".t('Corporation Management')."</legend>
                      <div class=\"fieldset-wrapper\">".theme('table', $header, $rows_corporation_management)."</div>
                     </fieldset>

                     <fieldset class=\"collapsible collapsed\">
                      <legend>".t('Drones')."</legend>
                      <div class=\"fieldset-wrapper\">".theme('table', $header, $rows_drones)."</div>
                     </fieldset>

                     <fieldset class=\"collapsible collapsed\">
                      <legend>".t('Electronics')."</legend>
                      <div class=\"fieldset-wrapper\">".theme('table', $header, $rows_electronics)."</div>
                     </fieldset>

                     <fieldset class=\"collapsible collapsed\">
                      <legend>".t('Engineering')."</legend>
                      <div class=\"fieldset-wrapper\">".theme('table', $header, $rows_engineering)."</div>
                     </fieldset>

                     <fieldset class=\"collapsible collapsed\">
                      <legend>".t('Gunnery')."</legend>
                      <div class=\"fieldset-wrapper\">".theme('table', $header, $rows_gunnery)."</div>
                     </fieldset>

                     <fieldset class=\"collapsible collapsed\">
                      <legend>".t('Industry')."</legend>
                      <div class=\"fieldset-wrapper\">".theme('table', $header, $rows_industry)."</div>
                     </fieldset>

                     <fieldset class=\"collapsible collapsed\">
                      <legend>".t('Leadership')."</legend>
                      <div class=\"fieldset-wrapper\">".theme('table', $header, $rows_leadership)."</div>
                     </fieldset>

                     <fieldset class=\"collapsible collapsed\">
                      <legend>".t(' Missile Launcher Operation')."</legend>
                      <div class=\"fieldset-wrapper\">".theme('table', $header, $rows_missile_launcher_operation)."</div>
                     </fieldset>

                     <fieldset class=\"collapsible collapsed\">
                      <legend>".t('Learning')."</legend>
                      <div class=\"fieldset-wrapper\">".theme('table', $header, $rows_learning)."</div>
                     </fieldset>

                     <fieldset class=\"collapsible collapsed\">
                      <legend>".t('Mechanic')."</legend>
                      <div class=\"fieldset-wrapper\">".theme('table', $header, $rows_mechanic)."</div>
                     </fieldset>

                     <fieldset class=\"collapsible collapsed\">
                      <legend>".t('Navigation')."</legend>
                      <div class=\"fieldset-wrapper\">".theme('table', $header, $rows_navigation)."</div>
                     </fieldset>

                     <fieldset class=\"collapsible collapsed\">
                      <legend>".t('Science')."</legend>
                      <div class=\"fieldset-wrapper\">".theme('table', $header, $rows_science)."</div>
                     </fieldset>

                     <fieldset class=\"collapsible collapsed\">
                      <legend>".t('Social')."</legend>
                      <div class=\"fieldset-wrapper\">".theme('table', $header, $rows_social)."</div>
                     </fieldset>

                     <fieldset class=\"collapsible collapsed\">
                      <legend>".t('Spaceship Command')."</legend>
                      <div class=\"fieldset-wrapper\">".theme('table', $header, $rows_spaceship_command)."</div>
                     </fieldset>

                     <fieldset class=\"collapsible collapsed\">
                      <legend>".t('Subsystems')."</legend>
                      <div class=\"fieldset-wrapper\">".theme('table', $header, $rows_subsystems)."</div>
                     </fieldset>

                     <fieldset class=\"collapsible collapsed\">
                      <legend>".t('Trade')."</legend>
                      <div class=\"fieldset-wrapper\">".theme('table', $header, $rows_trade)."</div>
                     </fieldset><br /><br /><hr /><br /><br />

                     <fieldset class=\"collapsible collapsed\">
                      <legend>".t('Planet Management')."</legend>
                      <div class=\"fieldset-wrapper\">".theme('table', $header, $rows_pi)."</div>
                     </fieldset><br /><br /><hr /><br /><br />";


                 // Connect all of the html embedded skills with the rest
                 // of the page
                 $html[$key] = $html[$key].$output_of_skill_sheet;




      }

    // Connect all of the character sheets on the account togheter in one page
    foreach ((array) $html as $key => $value) {
        $output = $output.$value;
    }
          return ($output);
}

// Formatting function for ISK currency.

function convertcash($amount){
  $new_amount = "ISK ".sprintf("%.2f",$amount);
  return $new_amount;
}

?>