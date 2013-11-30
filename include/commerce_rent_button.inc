<?php

//Get user name
function _get_user_name($uid){
	$user_name = db_select('users', 'n') // check item is present in the rent item table
	->fields('n', array('name'))
	->condition('uid', $uid, '=')
	->execute()
	->fetchObject();
	
	return $user_name;
}

//Get user uid from the user table
function user_role_type($uid)
{
  $user_rid = db_select('users_roles', 'n')
		->fields('n', array('rid'))
		->condition('uid', $uid,'=')
		->execute()
		->fetchAssoc();
  return $user_rid;
}



//get item details according to its sku from master rent table
function user_rentproduct($sku)
{
  $result_product_rent = db_select('commerce_rent', 'n')
		->fields('n', array())
		->condition('sku', $sku, '=')
		->execute()
		->fetchAssoc();
  return $result_product_rent;
}

//compare user max value and total no of user item.
function max_value_status($uid)
{
	$rid = user_role_type($uid);
	
	$normal_user = db_select('commerce_rent_user_setting', 'r')
		->fields('r', array('commerce_rent_user_max'))
		->condition('rid', $rid, '=')
		->execute()
		->fetchObject();
	
	$total_user_item = db_select('commerce_rent_item_details', 'r')
		->fields('r', array())
		->condition('commerce_rent_item_request_uid', $uid, '=')
		->condition('commerce_rent_item_status', 'OPEN')
		->execute()
		->rowCount();		
		
	if(!empty($normal_user))
	{
		if($normal_user->commerce_rent_user_max == $total_user_item)
		{
			return FALSE;
		}
		else
		{
			return	TRUE;
		}
	}
}

//return item position for new item is added to its bookshelf.
function rent_position_max_value($uid)
{
	$query_max = db_select('commerce_rent_item_details', 's');
	$query_max->addExpression('MAX(commerce_rent_item_position)');
	$query_max->condition('commerce_rent_item_request_uid', $uid, '=');
	$query_max->condition('commerce_rent_item_status', 'OPEN', '=');
	$max = $query_max->execute()->fetchObject();
	
	if(!empty($max->expression))
	{
		$item_position = ($max->expression+01);
	}
	else
	{
		$item_position = '001';
	}
	
	return $item_position;
}

//get user request id from the request table
function _get_user_user_request_id($uid){
	$rent_item_request_id = db_select('commerce_rent_item_user_details', 'n')
	->fields('n', array('commerce_rent_item_request_id'))
	->condition('commerce_rent_user_uid', $uid, '=')
	->condition('commerce_rent_item_status', 'OPEN', '=')
	->orderBy('commerce_rent_item_request_id', 'DESC')
	->range(0, 1)
	->execute()
	->fetchObject();
	
	return $rent_item_request_id;
}


//function which will call a form function and return a rent button to the node id.
function rent_button_block_form($sku, $nid)
{
	global $user;
  $result_product_rent = user_rentproduct($sku);
  $output = '';
  if(!empty($result_product_rent))
	{
    $output = drupal_get_form('form_rent_button', $result_product_rent['commerce_rent_item_sno'], $result_product_rent['title'], $result_product_rent['field_book_author_value'], $nid);
	}
  return drupal_render($output);
}

//form function
function form_rent_button($form, &$form_state, $rent_item_sno, $rent_item_name, $rent_item_author, $nid)
{
	global $user;
	global $base_url;
	
  $rent_item = db_select('commerce_rent_item_details', 'n')
		->fields('n', array('commerce_rent_item_sno'))
		->condition('commerce_rent_item_name', $rent_item_name, '=')
		->condition('commerce_rent_item_author', $rent_item_author, '=')
		->condition('commerce_rent_item_request_uid', $user->uid, '=')
		//->condition(db_or()->condition('commerce_rent_item_status', 'IN-P')->condition('commerce_rent_item_status', 'CLOSE'))
		->condition('commerce_rent_item_status', 'OPEN')
		->execute()
		->fetchAssoc();
		
	//get rid of the user	
	$user_join_library_now = db_select('role', 'n')
		->fields('n', array('rid'))
		->execute();
	
	//get all rid of the user in a blank array.
	while($result = $user_join_library_now->fetchObject())
	{
		$role[] = $result->rid;
	}
	
	$role_id = array(4, 6, 7, 8);
		
	//condition for checking rid in role array if the rid in role array then the user will not generated.
	if($user_role_id = in_array($role_id, $role, true) )
	{
		$url_join_library = '';
	}
	else
	{
		$url_join_library = 'membership-library-plans-prices';
	}
		
  if($user->uid)
	{
		if(!empty($rent_item))
		{
			$form['rent'] = array(
				'#value' => 'Added to Bookshelf',
				'#type' => 'submit',
				'#attributes' => array(
					'disabled'  => 'disabled',
				),
				'#prefix' => '<div class="commerce_rent_button_disabled">',
				'#suffix' => '</div>',
			);
		}
		else
		{
			$weight = -20;
			
			$destination = drupal_get_destination();
			$url = $base_url.'/rent/rent_button_link?item='.$rent_item_sno.'&destination=';
			$form['rent'] = array(
				'#type' => 'link',
				'#title' => t('Rent'),
				'#href' => $url.drupal_get_normal_path($_GET['q']),
				'#prefix' => '<div class="commerce_rent_button">',
				'#suffix' => '</div>',
			);
		}
		
		$form['join_library'] = array(
			'#type' => 'markup',	
			'#prefix' => '<div class="commerce_rent_join_library"><a href="'.$base_url.'/'.$url_join_library.'"> Join Library Now</a>',
			'#suffix' => '</div>',
		);

	}
  else
	{
		$destination = drupal_get_destination();
		$url = $base_url.'/user/register?current=';
		$form['rent_user_register'] = array(
			'#type' => 'link',
			'#title' => t('Rent'),
			'#href' => t($url.$destination['destination']),
			'#prefix' => '<div class="commerce_rent_button_disabled">',
			'#suffix' => '</div>',
		);
		
		$form['join_library'] = array(
			'#type' => 'markup',	
			'#prefix' => '<div class="commerce_rent_join_library"><a href="'.$base_url.'/'.$url_join_library.'"> Join Library Now</a>',
			'#suffix' => '</div>',
		);
		
  }
	
	return $form;
}

//function which will call when user add item to their bookshelf.
function _rent_button_link_fuctionality()
{
	global $user;
	global $base_url;
	
	//Get item sno
	$item_sno = $_GET['item'];
	
	//Get redirect path
	$redirect = $_GET['destination'];
	
	$rent_item = _item_status_in_user_item_table($item_sno);
		
	if( $rent_item == 'not-in-table')
	{
		//Get details of the item from master table
		$commerce_item = db_select('commerce_rent', 'n')
			->fields('n', array())
			->condition('commerce_rent_item_sno', $item_sno, '=')
			->execute()
			->fetchObject();
			
		//get user item position
		$rent_item_position_max_value = rent_position_max_value($user->uid);
				
		//get user name
		$user_name = _get_user_name($user->uid);
				
		//get user rid
		$rid = user_role_type($user->uid);
				
		$id = db_insert('commerce_rent_item_user_details')
			->fields( array(
				'commerce_rent_user_uid' => $user->uid,
				'commerce_rent_user_name' => $user_name->name,
				'commerce_rent_user_rid' => $rid['rid'],
				'commerce_rent_item_added_time' => REQUEST_TIME,
				)
			)
			->execute();
				
		//get request-id from commerce_rent_item_user_details table
		$get_user_user_request_id = _get_user_user_request_id($user->uid);
				
		$id = db_insert('commerce_rent_item_details')
			->fields(array(
				'commerce_rent_item_sku' => $commerce_item->sku,
				'commerce_rent_item_name' => $commerce_item->title,
				'commerce_rent_item_author' => $commerce_item->field_book_author_value,
				'commerce_rent_item_image_path' => $commerce_item->filename,
				'commerce_rent_item_position' => $rent_item_position_max_value,
				'commerce_rent_item_location' => $commerce_item->commerce_rent_item_location,
				'commerce_rent_item_request_id' => $get_user_user_request_id->commerce_rent_item_request_id,
				'commerce_rent_item_request_uid' => $user->uid,
				)
			)
			->execute();

	}
	else{
		dpm('Book is already in your Book-shelf.');
	}
	drupal_goto($base_url.$redirect);
	exit;
}

//form function which will check the product entry in commerce_rent table. 
function _product_status($product_sku)
{
	$item_status = db_select('commerce_rent', 'n')
	->fields('n', array('sku'))
	->condition('sku', $product_sku, '=')
	->execute()
	->fetchAssoc();
	
	if(!empty($item_status))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

function _item_status_in_user_item_table($item_sno)
{
	global $user;
	
	$sku = db_select('commerce_rent', 'n')
		->fields('n', array('sku'))
		->condition('commerce_rent_item_sno', $item_sno, '=')
		->execute()
		->fetchObject();
		
	$item = db_select('commerce_rent_item_details', 'n')
		->fields('n', array('commerce_rent_item_sno'))
		->condition('commerce_rent_item_sku', $sku->sku, '=')
		->condition('commerce_rent_item_status', 'OPEN')
		->condition('commerce_rent_item_request_uid', $user->uid, '=')
		->execute()
		->fetchObject();
	
	$status = isset($item->commerce_rent_item_sno) ? 'in-table' : 'not-in-table';
	
	return $status;
}