<?php
//form function for configutaion menu item which will dispaly all the user in table format.
function rent_config_form()
{
$form = array();
	$form[0] = drupal_get_form('rent_config_form_user_insert');
	$form[1] = drupal_get_form('rent_config_form_user_list');

  return drupal_render($form);
}

//function which will generate form to insert a user configuration value.
function rent_config_form_user_insert($form, &$form_state)
{
	//getting authenticated user list.
  $option_val = commerce_rent_config_dropdown_options();
  
	$form = array();  
  $form['user'] = array(
    '#prefix' => '<table id="check" border="1" cellpadding="0" cellspacing="0">',
    '#suffix' => '</table>',
    );
  $form['user']['heading'] = array(
    '#title' => t('User type'),
    '#type' => 'item',
    '#prefix' => '<tr><th width="20%" colspan="4">',
    '#suffix' => '</th>',
    );
  $form['user']['rid'] = array(
    '#title' => t('Authenticate'),
    '#type' => 'select',
    '#required' => TRUE,
    '#options' => $option_val,
    '#prefix' => '<tr><td width="20%">',
    '#suffix' => '</td>',
    );
	$form['user']['min'] = array(
    '#title' => t('Min'),
    '#type' => 'textfield',
    '#prefix' => '<td width="20%">',
    '#suffix' => '</td>',
    //'#required' => TRUE,
    '#description' => t('Please min value.'),
    '#attributes' => array(
    'style' => array(
      'position: relative;',
      'width: 20%;'
        ),
      ),
    );
	
	/*
	$form['user']['max'] = array(
    '#title' => t('Max'),
    '#type' => 'textfield',
    '#prefix' => '<td width="20%">',
    '#suffix' => '</td>',
    //'#required' => TRUE,
    '#description' => t('Please max value.'),
    '#attributes' => array(
      'style' => array(
        'position: relative;',
        'width: 20%;'
          ),
        ),
      );
	*/
	
  $form['user']['type'] = array(
    '#title' => t('Eligibility'),
    '#type' => 'textfield',
    '#prefix' => '<td width="20%">',
    '#suffix' => '</td>',
    //'#required' => TRUE,
    '#description' => t('Please eligibility value.'),
    '#attributes' => array(
      'style' => array(
        'position: relative;',
        'width: 20%;'
          ),
        ),
      );
  $form['user']['submit'] = array(
    '#value' => 'Submit',
    '#type' => 'submit',
    '#submit' => array('rent_user_setting_add'),
    '#prefix' => '<td width="20%">',
    '#suffix' => '</td></tr>',
     );
		 
	$form['description'] = array(
    '#title' => t('Note'),
    '#type' => 'markup',
    '#prefix' => '<div><span style="font-weight: bold;">NOTE:</span> Maximum value for all the authenticated user is 30 (which is fixed).',
    '#suffix' => '</div>',
     );
return $form;

}

//form function which will generate table user list.
function rent_config_form_user_list($form, &$form_state)
{
	$form['user_display'] = array(
		'#type' => 'actions',
    '#prefix' => '<table id="check" border="1" cellpadding="0" cellspacing="0">',
    '#suffix' => '</table>',
  );
  
	$form['user_display']['user_heading'] = array(
    '#type' => 'markup',
    '#prefix' => '<tr><th>User</th><th>Min</th><th>Eligiblity</th><th>Action</th>',
    '#suffix' => '</th></tr>',
  );
  
	$i = 0;
	
	//Query which will fetch all the data from the config table.
	$results = db_select('commerce_rent_user_setting', 'c')
    ->fields ('c', array())
    ->execute();
  
	while($record = $results->fetchObject())
	{
		if(isset($record))
		{
			$form['user_display']['user_title'.$i] = array(
				'#type' => 'markup',
				'#prefix' => '<tr><td>'.ucfirst($record->name),
				'#suffix' => '</td>',
			);
			
			$form['user_display']['user_min'.$i] = array(
				'#type' => 'markup',
				'#prefix' => '<td>'.$record->commerce_rent_user_min,
				'#suffix' => '</td>',
			);
			
			$form['user_display']['user_eligible'.$i] = array(
				'#type' => 'markup',
				'#prefix' => '<td>'.$record->commerce_rent_user_eligible,
				'#suffix' => '</td>',
			);
			
			$form['user_display']['submit'.$i] = array(
				'#type' => 'submit',
				'#value' => 'Delete',
				'#prefix' => '<td>',
				'#suffix' => '</td></tr>',
				'#submit' => array('delete_config_item'),
				'#attributes' => array(
					'name' => 'submit'.$i,
				),
			);
			
			$form['user_display']['user'.$i] = array(
				'#type' => 'hidden',
				'#value' => $record->rid,
			);
			
			$i++;
		}
	}
		$form['user_display']['count'] = array(
			'#type' => 'hidden',
			'#value' => $i,
		);

return $form;
}

//function which will call by form submit button for store the max and min value in the table according to the user role.
function rent_user_setting_add()
{
	// array which will store the post data.
  $rid = $_POST['rid'];
	
	//check condition for user rid.
	if(!empty($rid))
	{
		//query which will slect the role name from role table.
		$results = db_select('role', 'o')
			->fields( 'o', array('name',) )
			->condition('rid', $rid)
			->execute()
			->fetchAssoc();

		//query which will store the post data in commerce_rent_user_setting table.
		$num_deleted2 = db_insert('commerce_rent_user_setting')
			->fields(array( 
				'rid' => $_POST['rid'],
				'name' => ucfirst($results['name']),
				'commerce_rent_user_min' => $_POST['min'],
				'commerce_rent_user_max' => '30',
				'commerce_rent_user_eligible' => $_POST['type'],
				)
      )
			->execute();
		
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

//function which will call by form for drop-down role options.
function commerce_rent_config_dropdown_options()
{
  $re = array();
  $re2 = array();
  
  //query which will select all name form role table and store in the array.
  $query1 = db_query('SELECT rid, name FROM {role} WHERE name != :member AND name != :user', array(':member' => 'corporate member', ':user' => 'corporate user'));
	
  while($record = $query1->fetchObject())
	{
    $re[$record->rid] = ucfirst($record->name);  
  }
  
  //query which will select all name form commerce_rent_user_setting table and store in the array.
  $query2 = db_query('SELECT rid, name FROM {commerce_rent_user_setting}', array());
  while($record2 = $query2->fetchObject())
	{
    $re2[$record2->rid] = ucfirst($record2->name);  
  }
  
  //array which will store the role after comparison of two array.
  $result = array_diff($re, $re2);
  
	return $result; 
}

//delete user from the table list.
function delete_config_item()
{
	for( $i=0; $i<$_POST['count']; $i++ )
	{
		if(isset($_POST['submit'.$i]))
		{
			if((isset($_POST['user'.$i])))
			{
				db_delete('commerce_rent_user_setting')
				->condition('rid', $_POST['user'.$i])
				->execute();
			}
		}
	}
}