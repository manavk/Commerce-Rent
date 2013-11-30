<?php
//Main function which initialize the form.
function rent_manage_form($form, &$form_state)
{
	$weight = 0;
	$query = db_select('commerce_rent_request', 'c')
  ->fields('c', array())
  ->condition('commerce_rent_user_request_status', 'OPEN', '=')
  ->execute();
	
	$form = array();
	$form['item_manage'] = array(
		'#type' => 'markup',
    '#prefix' => '<table>',
    '#suffix' =>'</table>',
		'#weight' => $weight++,
	);
	
	$form['item_manage']['mainheading'] = array(
		'#type' => 'markup',
    '#prefix' => '<tr><th style="background:gray;font-size:20px;" colspan="4">Manage Request</th>',
    '#suffix' =>'</tr>',
		'#weight' => $weight++,
	);
	
	$form['item_manage']['heading'] = array(
		'#type' => 'markup',
    '#prefix' => '<tr><th>Sno.</th><th>User Name</th><th>Request Date/Time</th><th>Action</th>',
    '#suffix' =>'</tr>',
		'#weight' => $weight++,
		);
	$i=1;
	while($result = $query->fetchObject())
	{
		$user_name = db_select('users', 'c')
			->fields('c', array('name'))
			->condition('uid', $result->commerce_rent_request_user_uid, '=')
			->execute()
			->fetchObject();
	
		$form['item_manage']['title'.$i] = array(
		'#type' => 'markup',
		'#prefix' => '<td>'.$i,
    '#suffix' =>'</td>',
		'#weight' => $weight++,
		);
		$form['item_manage']['author'.$i] = array(
		'#type' => 'markup',
		'#prefix' => '<td>'.$user_name->name,
    '#suffix' =>'</td>',
		'#weight' => $weight++,
		);
		$form['item_manage']['item_sno'.$i] = array(
		'#type' => 'markup',
		'#prefix' => '<td>'.$result->commerce_rent_user_request_time,
    '#suffix' =>'</td>',
		'#weight' => $weight++,
		);
		$form['item_manage']['submit'.$i] = array(
		'#type' => 'link',
		'#href' => 'admin/commerce/rent/request_manage/'.$result->commerce_rent_request_user_uid.'/'.$result->commerce_rent_request_id,
    '#title' => t('View Details'),
		'#prefix' => '<td>',
    '#suffix' =>'</td></tr>',
		'#weight' => $weight++,
		);
	$i++;	
	}
	
	return $form;
}


//function which used to display rent item by user id.
function request_close_form(){
  if($_POST)
	{
		$check = 0;
    $uid = arg(4);
		$request_id = arg(5);
    $book_sno = array();
    $book_location = array();
    $sku = array();
    $location = array();
    if(isset($_POST['count']))
		{
      for($i = 0; $i < $_POST['count']; $i++ )
			{
        if(isset($_POST['check'.$i]))
				{
          $book_sno[] = $_POST['s'.$i];
          $book_location[] = $_POST['s'.$i.'dropdown_second'];
					$check++;
        }
      }
		}
		
		//check status of delivery remaining value
		$remaining_delivery_status = commerce_rent_delivery_eligible($uid);
		
		//check condition of delivery remain in user account
		if($remaining_delivery_status == FALSE)
		{
			drupal_set_message('There is no delivery remains in this User\'s Account', 'error');
			drupal_goto('admin/commerce/rent/request_manage');
		}
		
		//check how many items are selected to send to the user
		$query_user_eligibility = user_eligibility($uid);

		//condition for checking selected item is equivalent to No of item eligible for or not.
		if(	$check > $query_user_eligibility )
		{
			drupal_set_message('Please select at least '.$query_user_eligibility.' item or less.', 'error');
			drupal_goto('admin/commerce/rent/request_manage/'.$uid.'/'.$request_id);
		}
		
    $a = '';
    $book_sno_filter = array_filter($book_sno);
    $book_location_filter = array_filter($book_location);
    $product_id = array();
    for($j=0; $j<count($book_sno_filter); $j++)
		{
      $query = db_query('SELECT * FROM commerce_rent WHERE commerce_rent_item_sno = :uid', array(':uid' => $book_sno_filter[$j]));
      
			$record = $query->fetchAll(PDO::FETCH_ASSOC);
			
			$query_pos = db_query('SELECT a.commerce_rent_item_position FROM {commerce_rent_item_details} a WHERE a.commerce_rent_item_request_uid = :user AND (a.commerce_rent_item_name = :title AND a.commerce_rent_item_author = :author) ORDER BY a.commerce_rent_item_position ASC', array( ':user' => $uid,':title' => $record[0]['title'], ':author' => $record[0]['field_book_author_value']));
      $record_pos = $query_pos->fetchAssoc();
			
      $sku[] = $record[0]['sku']; 
      $location[] = $record[0]['commerce_rent_item_location']; 
			
			$product = commerce_product_load_by_sku($record[0]['sku']);
			
      $variables = array(
        'path' => $product->field_book_image['und'][0]['uri'],
        'width' => '30%',
        'height' => '15%',
        'alt' => $product->title,
        'title' => $product->title,
        'attributes' => array('class' => 'some-img'),
      );
      $img = theme('image', $variables);
      $a .= "<tr>";
      $a .= "<td>".$product->title."</td>";
      $a .= "<td>".$product->field_author['und'][0]['value']."</td>";
      $a .= "<td align='center'>".$img."</td>";
      $a .= "<td>".$record_pos['commerce_rent_item_position']."</td>";
      $a .= "<td>".$record[0]['commerce_rent_item_sno']."</td>";
      $a .= "<td>".$record[0]['commerce_rent_item_location']."</td>";
      $a .= "</tr>";
		}
    $abc;
		$abc = _user_subscription_details_right($uid);
		$abc .= user_details_right($uid);
		$abc .= previous_five_details($uid);
    $abc .= "<table style='width: 63%;' border='0' cellspacing='0' cellpadding='0'>";
    $abc .= "<tr>";
    $abc .= "<th style='font-size: 12px;text-align:center;'>Title</th>";
    $abc .= "<th style='font-size: 12px;text-align:center;'>Author</th>";
    $abc .= "<th style='font-size: 12px;text-align:center;'>Image</th>";
    $abc .= "<th style='font-size: 12px;text-align:center;'>Position</th>";
    $abc .= "<th style='font-size: 12px;text-align:center;'>Sno.</th>";
    $abc .= "<th style='font-size: 12px;text-align:center;'>Location</th>";
    $abc .= "</tr>";
    $abc .= $a;
    $abc .= "</table>";
    
    //form function which will call another function and that will return a form
    $abc .= shipment_form($post = $book_sno_filter, $sku ,$uid = $uid, $location );
     return $abc;
  }
  else
	{
    $uid = arg(4);
		
    $query = db_query('SELECT a.*, b.* FROM {commerce_rent_item_details} a, {commerce_rent_item_user_details} b WHERE a.commerce_rent_item_request_id = b.commerce_rent_item_request_id AND a.commerce_rent_item_request_uid = :uid AND b.commerce_rent_user_uid = :uid AND a.commerce_rent_item_status = :status AND b.commerce_rent_item_status = :status ORDER BY a.commerce_rent_item_position ASC', array(':uid' => $uid, ':status' => 'OPEN'));
    $record = $query->fetchAll(PDO::FETCH_ASSOC);
		
		$abc;
		$abc = _user_subscription_details_right($uid);
    $abc .= user_details_right($uid);
    
		$abc .= previous_five_details($uid);
    $abc .= manage_form_drupal($count = count($record), $uid);
    return $abc;
    
  }
  
  }

//form function which will call a form.
function shipment_form( $post, $sku, $uid, $location )
{
  $return_form = drupal_get_form('shipment_get_form', $post, $sku, $uid, $location);
  return drupal_render($return_form);
}

//function which return form to shipment_form
function shipment_get_form($form, &$form_state, $post, $sku, $uid, $location)
{
  $form = array();
  $form['item'] = array(
    '#type' => 'markup',
    '#prefix' => '<table style="width: 36%;float: right; clear:both;">',
    '#suffix' => '</table>',
  );
  $i = 0;
  foreach($post as $id)
	{
    $form['item']['book'.$i] = array(
      '#type' => 'hidden',
      '#value' => $id,
    );
    $i++;
  }
  $j=0;
  foreach($sku as $sku)
	{
    $form['item']['sku'.$j] = array(
      '#type' => 'hidden',
      '#value' => $sku,
    );
    $j++;
  }
	$k=0;
  foreach($location as $loc)
	{
    $form['item']['location'.$k] = array(
      '#type' => 'hidden',
      '#value' => $loc,
    );
    $k++;
  }
	$form['item']['count'] = array(
    '#type' => 'hidden',
    '#value' => $i,
	);
  $form['item']['uid'] = array(
    '#type' => 'hidden',
    '#value' => $uid,
  );
  
	//get current date to generate unique item sno.
	$current_date = REQUEST_TIME;
	
	//split current date
	$arr2 = str_split($current_date, 4);
	
	//check the pattern of the date in the database table.
	$q = db_select('commerce_rent_shipping_details', 'st');
	$q->condition('commerce_rent_shipping_request_close_time', '%'.date('Y-m-d').'%', 'LIKE');
  $q->addExpression('MAX(commerce_rent_shipping_shipment_no)');
  $max = $q->execute()->fetchAssoc();
	
	//generate unique sno for the shipment item table
	if(isset($max['expression'])){
    $value = explode("IRLTBS".date('ymd'), $max['expression']);
    if(isset($value[1]))
		{
      $ship_no = 'IRLTBS'.date('ymd').sprintf("%03d", ($value[1]+001));
    }
  }
  else
	{
    $ship_no = 'IRLTBS'.date('ymd').'001';
  }
	
	$form['item']['heading'] = array(
    '#type' => 'markup',
    '#prefix' => '<tr><th style="background:gray;font-size:15px;">Shippment details',
    '#suffix' => '</th></tr>',
  );
	$form['item']['shipment_no'] = array(
		'#title' => t('Unique Shipment No. Auto. Gen.'),
    '#type' => 'textfield',
    '#value' => $ship_no,
    '#attributes' => array(
			'disabled' => 'disabled',
			'style' => array('font-family: none;'),
		),
    '#prefix' => '<tr><th>',
    '#suffix' => '</th></tr>',
  );
  $form['item']['shipment_auto_no'] = array(
    '#type' => 'hidden',
    '#value' => $ship_no,
  );
  $form['item']['courier_company'] = array(
    '#title' => t('Courier company'),
    '#type' => 'textfield',
		'#attributes' => array(
			'style' => array('font-family: none;'),
		),
    '#prefix' => '<tr><th>',
    '#suffix' => '</th></tr>',
  );
  $form['item']['weight'] = array(
    '#title' => t('Weight'),
    '#type' => 'textfield',
		'#attributes' => array(
			'style' => array('font-family: none;'),
		),
    '#value' => '0.0',
    '#prefix' => '<tr><th>',
    '#suffix' => '</th></tr>',
  );  
  $form['item']['tracking_id'] = array(
		'#title' => t('Tracking id'),
    '#type' => 'textfield',
		'#attributes' => array(
			'style' => array('font-family: none;'),
		),
    '#prefix' => '<tr><th>',
    '#suffix' => '</th></tr>',
  );
  $form['item']['tracking_url'] = array(
    '#title' => t('Tracking url'),
    '#type' => 'textfield',
		'#attributes' => array(
			'style' => array('font-family: none;'),
		),
    '#prefix' => '<tr><th>',
    '#suffix' => '</th></tr>',
  );
  $form['item']['dimension'] = array(
    '#title' => t('Dimension'),
    '#type' => 'textfield',
		'#attributes' => array(
			'style' => array('font-family: none;'),
		),
    '#prefix' => '<tr><th>',
    '#suffix' => '</th></tr>',
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#prefix' => '<div style="margin-bottom:10px;clear: both; float:right;">',
    '#suffix' => '</div>',
  );
  return $form;
}

//return rendered form array
function manage_form_drupal($count, $uid)
{
  $output = drupal_get_form('manage_item_content_form', $count = $count, $uid = $uid);
  return drupal_render($output);
}
	
//function which use to display item list with select box. Admin will select the item with its unqiue sno and sent to the user. 
function manage_item_content_form($form, $form_state, $count, $uid)
{
	global $base_path, $base_url;
	
  $query = db_query('SELECT a.*, b.* FROM {commerce_rent_item_details} a, {commerce_rent_item_user_details} b WHERE a.commerce_rent_item_request_id = b.commerce_rent_item_request_id AND a.commerce_rent_item_request_uid = :uid AND b.commerce_rent_user_uid = :uid AND a.commerce_rent_item_status = :status AND b.commerce_rent_item_status = :status ORDER BY a.commerce_rent_item_position ASC', array(":uid" => $uid, ":status" => "OPEN"));

  $record = $query->fetchAll(PDO::FETCH_ASSOC);
	
	$option = array();
  $form = array();	
  $weight = 0;
  $form['table2'] = array(
    '#type' => 'markup',
    '#prefix' => '<table style="width: 63%;" border="0" cellspacing="0" cellpadding="0">',
    '#suffix' =>'</table>',
    '#weight' => $weight++,
      );
	$form['table2']['th'] = array(
    '#type' => 'markup',
    '#prefix' => '<tr><th></th><th>Title</th><th>Author</th><th>Image</th><th>Position</th><th>Sno.</th><th>Location',
    '#suffix' =>'</th></tr>',
    );
  for($i=0; $i<$count; $i++){
	
	$product_id = db_select('commerce_rent', 'n')
		->fields('n', array('product_id'))
		->condition('field_book_author_value', $record[$i]['commerce_rent_item_author'])
		->condition('title', $record[$i]['commerce_rent_item_name'])
		->execute()
		->fetchObject();

	$product = commerce_product_load_by_sku($record[$i]['commerce_rent_item_sku']);
	
	//get all options for rent item sno
	$options_first = _ajax_test_get_first_dropdown_options($record[$i]['commerce_rent_item_author'], $record[$i]['commerce_rent_item_name']);
	
	$option_status = !empty($options_first) ? 1 : 0;
	switch($option_status)
	{
    case 1:
			$form['table2'][$i]['check'.$i] = array(
				'#type' => 'checkbox',
				'#value' => '',
				'#prefix' => '<tr><td>',
				'#suffix' =>'</td>',
      );
		break;
		
		case 0:
			$form['table2'][$i]['check'.$i] = array(
				'#type' => 'checkbox',
				'#value' => '',
				'#attributes' => array(
					'disabled' => 'disabled',
				),
				'#prefix' => '<tr><td>',
				'#suffix' =>'</td>',
      );
		break;
		
  } 
	 $form['table2'][$i]['item'] = array(
      '#type' => 'markup',
      '#prefix' => '<td>'.$record[$i]['commerce_rent_item_name'],
      '#suffix' =>'</td>',
        );
    $form['table2'][$i]['author'] = array(
      '#type' => 'markup',
      '#prefix' => '<td>'.$record[$i]['commerce_rent_item_author'],
      '#suffix' =>'</td>',
    );	
		
    $variables = array(
				'path' => $product->field_book_image['und'][0]['uri'],
				'width' => '40%',
				'height' => '15%',
				'alt' => $record[$i]['commerce_rent_item_name'],
				'title' => $record[$i]['commerce_rent_item_name'],
				'attributes' => array('class' => 'some-img', 'id' => 'my-img'),
			);
		
		$img = theme('image', $variables);
	
		//$img = l(theme('image', array('path'=>path_to_theme().$path->uri)), $base_url, array('html' => 'true'));    
	
		$form['table2'][$i]['img'] = array(
      '#type' => 'markup',
      '#prefix' => '<td align="center">'.$img,
      '#suffix' =>'</td>',
        );
    $form['table2'][$i]['position'] = array(
      '#type' => 'markup',
      '#prefix' => '<td>'.$record[$i]['commerce_rent_item_position'],
      '#suffix' =>'</td>',
        );
    
    $selected = isset($form_state['values']['s'.$i]) ? $form_state['values']['s'.$i] : key($options_first);
    $form['table2'][$i]['s'.$i] = array(
      '#type' => 'select',
      '#options' => $options_first,
      '#default_value' => $selected,
      '#ajax' => array(
        'event' => 'change',
        'callback' => 'ajax_test_dependent_dropdown_callback',
        'wrapper' => 'dropdown-second-replace'.$i,
          ),
      '#prefix' => '<td>',
      '#suffix' => '</td>',
        );
    $form['table2'][$i]['s'.$i.'dropdown_second'] = array(
      '#type' => 'textfield',
      '#size' => '2',
      '#value' => _ajax_test_second_dropdown_options($selected),
      '#default_value' => isset($form_state['values']['s'.$i.'dropdown_second']) ? $form_state['values']['s'.$i.'dropdown_second'] : '',
      '#prefix' => '<td style="border:none;"><div id="dropdown-second-replace'.$i.'">',
      '#suffix' =>'</div></td></tr>',
      '#attributes' => array(
        'style' => array(
          'border:none',
            ),
        'readonly' => 'readonly',
          ),
        );
    $form['count'] = array(
      '#type' => 'hidden',
      '#value' => $count,
        );
    $form['next'] = array(
      '#type' => 'submit',
      '#value' => 'Next',
      '#weight' => $weight++,
      '#prefix' => '<div id="submit">',
      '#suffix' =>'</div>',
      '#attributes' => array(
        'style' => array(
        'float:right; clear:both',
            ),
          ),
        );
    
  }
  return $form;
  
  }

//ajax function for drop-down option select which will tell the location.
function ajax_test_dependent_dropdown_callback($form, $form_state){
  $fields = $form_state['triggering_element']['#name'];
  $re = str_split($fields);
  return $form['table2'][$re[1]]['s'.$re[1].'dropdown_second'];
  
}

//ajax function
function _ajax_test_get_first_dropdown_options( $author, $item){
  $option = array();
  $query_sno = db_query('SELECT commerce_rent_item_sno FROM commerce_rent WHERE (field_book_author_value = :author AND title = :title) AND commerce_rent_item_availability = :status', array(':author' => $author, ':title' => $item, ':status' => 'IN'));
  while($record_sno = $query_sno->fetchObject()){
    $option[$record_sno->commerce_rent_item_sno] = $record_sno->commerce_rent_item_sno;  
    
  }
  return drupal_map_assoc($option);
  
  }

//ajax function
function _ajax_test_second_dropdown_options($key = ''){
  $option = array();
  $query_sno = db_query('SELECT commerce_rent_item_sno, commerce_rent_item_location FROM commerce_rent', array());
  while($record_loc = $query_sno->fetchObject()){
    $option[$record_loc->commerce_rent_item_sno] = $record_loc->commerce_rent_item_location;  
    
  }
  if (isset($option[$key]))
	{
    return $option[$key];
    
  }
  else {
    return array();
    
  }
  
  }

//display all item Sno. in drop-down option.
function first_options($author, $item){
$option = array();
$query_sno = db_query('SELECT commerce_rent_item_sno FROM commerce_rent WHERE field_book_author_value = :author AND title = :title ', array( ':author' => $author, ':title' => $item));
while($record_sno = $query_sno->fetchObject()){
  $option[$record_sno->commerce_rent_item_sno] = $record_sno->commerce_rent_item_sno;
  
}
return $option;
}

function user_details_right($uid)
{
	$user_result = drupal_get_form('user_details_rightform', $uid);
	return drupal_render($user_result);
}

function user_details_rightform($form, $form_state, $uid)
{
	$form = array();

	$weight = 0;
	$form = array();
	
	//get user eligibility value
	$query_user_eligibility = user_eligibility($uid);
	
	//get user name, address, mail, mobile etc.
	module_load_include('inc', 'commerce_rent', 'include/user_details');
	$user_address = user_address($uid);
	$user_name = user_full_name($uid);
	$user_mobile = user_mobile($uid);
	$user_mail = _user_mail($uid);
	$user_no_of_delivery_eligible = _user_no_of_delivery_eligible($uid);
	
	$form['table_user_rightside'] = array(
		'#type' => 'markup',
    '#prefix' => '<table style="width: 36%;float: right;clear: both;" border="0" cellspacing="0" cellpadding="0">',
    '#suffix' =>'</table>',
		'#weight' => $weight++,
		);
	$form['table_user_rightside']['mainheading'] = array(
		'#type' => 'markup',
    '#prefix' => '<tr><th style="background:gray;font-size:15px;" colspan="4">User Detail</th>',
    '#suffix' =>'</tr>',
		'#weight' => $weight++,
		);
	
	$form['table_user_rightside']['user_name'] = array(
		'#type' => 'markup',
    '#prefix' => '<tr><th style="width:45%;">user name</th><td>'.ucfirst($user_name['field_user_first_name_value']).' '.ucfirst($user_name['field_user_last_name_value']).'</td>',
    '#suffix' =>'</tr>',
		'#weight' => $weight++,
		);
	
	$form['table_user_rightside']['subscription'] = array(
		'#type' => 'markup',
    '#prefix' => '<tr><th>No of delivery (Eligible for)</th><td>'.$query_user_eligibility.'</td>',
    '#suffix' =>'</tr>',
		'#weight' => $weight++,
		);
	$form['table_user_rightside']['address'] = array(
		'#type' => 'markup',
    '#prefix' => '<tr><th>address</th><td>'.$user_address['field_user_address_thoroughfare'].', '.$user_address['field_user_address_premise'].', '.$user_address['field_user_address_locality'].'-'.$user_address['field_user_address_postal_code'].' '.$user_address['field_user_address_administrative_area'].'</td>',
    '#suffix' =>'</tr>',
		'#weight' => $weight++,
		);
		
	$form['table_user_rightside']['no_of_delivery'] = array(
		'#type' => 'markup',
    '#prefix' => '<tr><th>Mobile</th><td>'.$user_mobile['field_user_mobile_number_value'].'</td>',
    '#suffix' =>'</tr>',
		'#weight' => $weight++,
		);
		
		$form['table_user_rightside']['mobile'] = array(
		'#type' => 'markup',
    '#prefix' => '<tr><th>No. of Delivery Remain</th><td>'.$user_no_of_delivery_eligible->field_no_of_delivery_eligible_value.'</td>',
    '#suffix' =>'</tr>',
		'#weight' => $weight++,
		);
		
		$form['table_user_rightside']['mail'] = array(
		'#type' => 'markup',
    '#prefix' => '<tr><th>mail</th><td>'.$user_mail['mail'].'</td>',
    '#suffix' =>'</tr>',
		'#weight' => $weight++,
		);
	
	return $form;
}

function previous_five_details($uid)
{
	$user_result = drupal_get_form('previous_details', $uid);
	return drupal_render($user_result);
}


function previous_details($form, $form_state, $uid)
{
	$request_sno = library_request_sno($uid);
	$i = 0;
	$item_pos = array();
	$item_closing_date = array();
	
	while(isset($request_sno[$i]))
	{
		$item_closing_date[] = item_closing_date($uid, $request_sno[$i]);
		$item_sno = library_item_sno($uid, $request_sno[$i]);
		$item_pos[] = _item_position($item_sno);
		$i++;
	}	
	
	$form = array();
	$weight = 0;
	$form['table'] = array(
		'#type' => 'markup',
    '#prefix' => '<table style="width: 36%;float:right;clear:both;" border="0" cellspacing="0" cellpadding="0">',
    '#suffix' =>'</table>',
		'#weight' => $weight++,
		);
	$form['table']['mainheading'] = array(
		'#type' => 'markup',
    '#prefix' => '<tr><th style="background:gray;font-size:15px;" colspan="4">Previous Details</th>',
    '#suffix' =>'</tr>',
		'#weight' => $weight++,
		);
	
	if(empty($item_pos))
	{
		$form['table']['empty_row'] = array(
			'#type' => 'markup',
			'#prefix' => '<tr><td>There is no recent data for this user.</td>',
			'#suffix' =>'</tr>',
			'#weight' => $weight++,
		);
	}
	
	if(isset($item_pos[0]))
	{
		$form['table']['first_recent'] = array(
			'#type' => 'markup',
			'#prefix' => '<tr><th rowspan="2">First Recent</th><td>Position: '.$item_pos[0].'</td></tr><tr><td>Date: '.$item_closing_date[0].'</td>',
			'#suffix' =>'</tr>',
			'#weight' => $weight++,
			);
	}
	
	if(isset($item_pos[1]))
	{
		$form['table']['second_recent'] = array(
			'#type' => 'markup',
			'#prefix' => '<tr><th rowspan="2">second Recent</th><td>Position: '.$item_pos[1].'</td></tr><tr><td>Date: '.$item_closing_date[1].'</td>',
			'#suffix' =>'</tr>',
			'#weight' => $weight++,
			);
	}
	if(isset($item_pos[2]))
	{
		$form['table']['third_recent'] = array(
			'#type' => 'markup',
			'#prefix' => '<tr><th rowspan="2">third Recent</th><td>Position: '.$item_pos[2].'</td></tr><tr><td>Date: '.$item_closing_date[2].'</td>',
			'#suffix' =>'</tr>',
			'#weight' => $weight++,
		);
	}
	if(isset($item_pos[3]))
	{
		$form['table']['forth_recent'] = array(
			'#type' => 'markup',
			'#prefix' => '<tr><th rowspan="2">forth recent</th><td>Position: '.$item_pos[3].'</td></tr><tr><td>Date: '.$item_closing_date[3].'</td>',
			'#suffix' =>'</tr>',
			'#weight' => $weight++,
		);
	}
	if(isset($item_pos[4]))
	{
		$form['table']['fifth_recent'] = array(
			'#type' => 'markup',
			'#prefix' => '<tr><th rowspan="2">fifth recent</th><td>Position: '.$item_pos[4].'</td></tr><tr><td>Date: '.$item_closing_date[4].'</td>',
			'#suffix' =>'</tr>',
			'#weight' => $weight++,
		);
	}
	return $form;
}


function _user_subscription_details_right($uid)
{
	$user_result = drupal_get_form('_user_subscription_details_rightform', $uid);
	return drupal_render($user_result);
}

function _user_subscription_details_rightform($form, &$form_state, $uid)
{
	$result = db_select('field_data_field_commerce_sp_validity', 'n')
		->fields('n', array('field_commerce_sp_validity_value'))
		->condition('entity_id', $uid, '=')
		->execute()
		->fetchObject();
	$weight = 0;
	$form = array();
	
	$form['table_user_rightside'] = array(
		'#type' => 'markup',
    '#prefix' => '<table style="width: 36%;float: right;" border="0" cellspacing="0" cellpadding="0">',
    '#suffix' =>'</table>',
		'#weight' => $weight++,
		);
	$form['table_user_rightside']['mainheading'] = array(
		'#type' => 'markup',
    '#prefix' => '<tr><th style="background:gray;font-size:15px;" colspan="4">Subscription validity</th>',
    '#suffix' =>'</tr>',
		'#weight' => $weight++,
		);
	$form['table_user_rightside']['subscription_validity'] = array(
		'#type' => 'markup',
    '#prefix' => '<tr><th style="width:45%;">subscription validity</th><td>'.date('Y-M-d h:m', $result->field_commerce_sp_validity_value).'</td>',
    '#suffix' =>'</tr>',
		'#weight' => $weight++,
		);
	return $form;
}


function user_eligibility($uid)
{
	module_load_include('inc', 'commerce_rent', 'include/commerce_rent_button');
	$user_rid = user_role_type($uid);
	
	$query_user_eligibility = db_query('SELECT commerce_rent_user_eligible FROM commerce_rent_user_setting WHERE rid = :rid', array(':rid' => $user_rid['rid']))
	->fetchObject();
	
	return $query_user_eligibility->commerce_rent_user_eligible;
}

function library_request_sno($uid)
{
	$request_sno = array();
	$query_sno = db_query('SELECT commerce_rent_request_sno FROM commerce_rent_request WHERE commerce_rent_request_user_uid = :uid AND commerce_rent_user_request_status = :status  ORDER BY commerce_rent_user_request_time ASC LIMIT 5', array(':uid' => $uid, ':status' => 'IN-P'));
	$i = 0;
	while($sno = $query_sno->fetchObject())
	{
		$request_sno[$i] = $sno->commerce_rent_request_sno;
		$i++;
	}
	return $request_sno;
}

function item_closing_date($uid, $request_sno)
{
	$request_time = db_select('commerce_rent_shipping_details', 'n')
		->fields('n', array('commerce_rent_shipping_request_close_time'))
		->condition('commerce_rent_request_sno', $request_sno, '=')
		->condition('commerce_rent_shipping_user_uid', $uid, '=')
		->execute()
		->fetchObject();
		
		return $request_time->commerce_rent_shipping_request_close_time; 
}

function library_item_sno($uid, $request_sno)
{	
	$item_sno = array();
	$query = db_query('SELECT commerce_rent_shipping_item_sno FROM commerce_rent_shipping_details WHERE commerce_rent_request_sno = :request_id AND commerce_rent_shipping_user_uid = :uid AND commerce_rent_item_status = :status ORDER BY commerce_rent_shipping_request_close_time ASC', array(':uid' => $uid, ':request_id' => $request_sno, ':status' => 'OUT'));
	while($result = $query->fetchObject())
	{
		$item_sno[] = $result->commerce_rent_shipping_item_sno;
	}
	return $item_sno;
}

function _item_position($item_sno)
{
	$i = 0;
	$position = '';
	while(isset($item_sno[$i]))
	{
	$query = db_query('SELECT commerce_rent_item_position FROM commerce_rent_item_details WHERE commerce_rent_item_rent_sno = :request_id AND commerce_rent_item_status = :status', array(':request_id' => $item_sno[$i], ':status' => 'IN-P'));
	$result = $query->fetchObject();
	if(empty($position))
	{
		$position = $result->commerce_rent_item_position;
	}
	else
	{
		$position .= ', '.$result->commerce_rent_item_position;
	}
	
		$i++;
	}
	return $position;
}

function shipment_get_form_submit()
{
	global $user;
	
	//get user_id, user request id
	$user_id = arg(4);
	$user_request_id = arg(5);
	
	//get user details
	$user_details = user_load($user->uid);
	
	$remaining_delivery_status = commerce_rent_delivery_eligible($user_id);
	
	if($remaining_delivery_status == FALSE)
	{
		drupal_set_message('No. of delivery 0 in this user account');
		drupal_goto('admin/commerce/rent/request_manage');
	}
	
	//$data = array('user_id' => arg(4), 'request_id' => $commerce_request_sno->commerce_rent_request_sno);
	//module_invoke_all('rent_request_close', $data);	
		
	//Load "commerce rent user status" module for calling _commerce_rent_user_status function.
	module_load_include('inc', 'commerce_rent_user_status', 'include/commerce_rent_user_status');
		
	//defining variable and array for of list for containing item sno, sku, location, count
	$book_sno = array();
	$book_sku = array();
	$book_location = array();
	$count = $_POST['count'];
	$shipment_details = array();

  for($i=0; $i<$_POST['count']; $i++)
	{
    $book_sno[] = $_POST['book'.$i];
		$book_sku[] = $_POST['sku'.$i];
		$book_location[] = $_POST['location'.$i];
  }
	
	$shipment_details['courier_company'] = $_POST['courier_company'];
	$shipment_details['shipment_auto_no'] = $_POST['shipment_auto_no'];
	$shipment_details['tracking_id'] = $_POST['tracking_id'];
	$shipment_details['tracking_url'] = $_POST['tracking_url'];
	$shipment_details['weight'] = $_POST['weight'];
	$shipment_details['dimension'] = $_POST['dimension'];
	
	//calling this function to check the status, details of the user and insert value in commerce_rent_user_item_details, commerce_rent_shipping_details, commerce_rent_user_status table etc.
	_commerce_rent_user_status($user_id, $user_request_id, $book_sno, $book_sku, $book_location, $count, $shipment_details);
	
	
	/*********send mail to the current user start***************/
	
	// set up the to and from
	$to = $user_details->mail;
	$email_from = "customercare@indiareads.com";

	$params['request_no'] = $user_request_id;
	$params['name'] = $user_details->field_user_first_name['und'][0]['value'].' '.$user_details->field_user_last_name['und'][0]['value'];
	foreach( $book_sku as $sku)
	{
		$params['books_sku'][] = $sku;
	}

	//drupal mail function when the user submit their request
	drupal_mail('user_manage_request_close_mailer', 'key', $to, language_default(), $params, $email_from, TRUE);
	
	/*********send mail to the current user end***************/
	
	drupal_goto('admin/commerce/rent/request_manage', array() );
}

function commerce_rent_delivery_eligible($uid)
{
	$query_user_delivery = db_query('SELECT d.field_no_of_delivery_eligible_value FROM field_data_field_no_of_delivery_eligible d WHERE d.entity_id = :id', array(':id' => $uid));
  $result = $query_user_delivery->fetchObject();
	
	return ($result->field_no_of_delivery_eligible_value > 0) ? TRUE : FALSE;	
}


//function call by drupal_mail
function user_manage_request_close_mailer_mail($key, &$message, $params)
{
	$vars = array();
	$items_details = '';
	$i = 1;
	foreach($params['books_sku'] as $sku)
	{
		$product = commerce_product_load_by_sku($sku);
		
		$items_details .= '<tr>
					<td>
					<span style="font:normal 14px/1.4 verdana,​helvetica,​arial,​sans-serif; color: #333;">'.$i.'.<a href="#">'.$product->title.'</a></span>		
					</td>				
				</tr>';
		$i++;
	}
	
	$language = $message['language'];
	
	switch($key) {
		//switching on $key lets you create variations of the email based on the $key parameter
		case 'key':
			$message['subject'] = t('IndiaReads Customer Care', $vars, $language->language); //$vars required even if not used to get $language in there
			//the email body is here, inside the $message array
			$message['body'][] = '<table cellpadding="5" cellspacing="0" width="600px" style="margin: 0 auto;" bgcolor="#fefefe">
				<tr>
					<td>
							<table cellpadding="0" cellspacing="0" width="100%"  bgcolor="#ffffff">
								<tr>
									<td align="left" cellpadding="0">
									<img width="200px" src="https://dl.dropboxusercontent.com/u/59598961/emailLogo.png" /></td>
									<td align="right" cellpadding="0">
										<span style="font:normal 14px/1.4 verdana,​helvetica,​arial,​sans-serif;">Call Us:
											<span style="color: #EF4026;">1800-103-7323</span>
										</span>
									</td>
								</tr>
							</table>					
					</td>				
				</tr>
				
				<tr>
					<td style="border-top: 2px dotted #ddd;">
					<span style="font:normal 14px/1.4 verdana,​helvetica,​arial,​sans-serif; color: #333;">Dear '.$params['name'].'</span>				
					</td>				
				</tr>
				
				<tr>
					<td>
					<span style="font:normal 14px/1.4 verdana,​helvetica,​arial,​sans-serif; color: #333;">We have despatched your library books for Library Request No. '.$params['request_no'].'</span>		
					</td>				
				</tr>
				
				<tr>
					<td>
					<span style="font:normal 14px/1.4 verdana,​helvetica,​arial,​sans-serif; color: #333;">Your books will be delivered to you within 3 working days</span>				
					</td>				
				</tr>
				
				<tr>
					<td>
					<span style="font:normal 14px/1.4 verdana,​helvetica,​arial,​sans-serif; color: #333;"><b>Details of books issued:</b></span>		
					</td>				
				</tr>'.$items_details.'

				<tr>
					<td>
					<span style="font:normal 14px/1.4 verdana,​helvetica,​arial,​sans-serif; color: #333;">
					If you have any books currently issued to you, please pack them in the return packaging provided and have them ready for pick-up.
					</span>		
					</td>				
				</tr>
				
				<tr>    
					<td>
					<span style="font:normal 14px/1.4 verdana,​helvetica,​arial,​sans-serif; color: #333;">If you have any queries or suggestions, please mail us at: <a href="mailto:customercare@indiareads.com">customercare@indiareads.com</a></span>		
					</td>				
				</tr>
				
				<tr>
					<td>
					<span style="font:normal 14px/1.4 verdana,​helvetica,​arial,​sans-serif; color: #333;">or call us at <span style="color: #EF4126;">1800-103-7323</span> (Mon to Sat - 10:00 am to 6:30pm)</span>		
					</td>				
				</tr>
				
				
				<tr>
					<td>
					<span style="font:normal 14px/1.4 verdana,​helvetica,​arial,​sans-serif; color: #333;">Regards</span>		
					</td>				
				</tr>
				
				<tr>
					<td>
					<span style="font:normal 14px/1.4 verdana,​helvetica,​arial,​sans-serif; color: #333;">IndiaReads.com Team</span>		
					</td>				
				</tr>
				
				<tr>
					<td style="border-top: 1px dotted #ddd;">
					<span style="font:normal 12px/1.4 verdana,​helvetica,​arial,​sans-serif; color: #333;">This is a system generated alert. Please do not to reply to this message.</span>
					</td>				
				</tr>

			</table>';
		break;
	}
}
