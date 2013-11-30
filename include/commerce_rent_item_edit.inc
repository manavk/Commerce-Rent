<?php

function _commerce_rent_item_edit_list()
{
	if(isset($_GET['sku']))
	{
		$data = _commerce_rent_item_edit_form($_GET['sku']);
	}
	else
	{
		$data= _commerce_rent_item_edit_form();
	}
	
	return $data;
}

//function returns list of rent items in table view
function _commerce_rent_item_edit_form( $sku = '' )
{
	global $base_url;
	
	$search_field = _commerce_rent_edit_item_search_field();
	$html = $search_field;
	
	//Create a list of headers for your Html table
	$header = array(
		array('data' => 'Sno.',),
		array('data' => 'Title', 'field' => 'title', 'sort' => 'asc'),
		array('data' => 'Author', 'field' => 'field_book_author_value'),
		array('data' => 'SKU', 'field' => 'sku'),
		array('data' => 'Location'),
		array('data' => 'Action'),
	);
	
	if(!empty($sku))
	{
		//Create the Sql query for table data
		$query = db_select('commerce_rent', 'n')
			->fields('n', array())
			->condition('sku', $sku)
			->extend('PagerDefault') 	//Pager Extender
			->extend('TableSort')		//Sorting Extender
			->orderByHeader($header)//Field to sort on is picked from $header
			->fields('n', array());

		$results = $query->execute();
	}
	else
	{
		//Create the Sql query for table data
		$query = db_select('commerce_rent', 'n')
			->fields('n', array())
			->extend('PagerDefault') 	//Pager Extender
			->extend('TableSort')		//Sorting Extender
			->orderByHeader($header)//Field to sort on is picked from $header
			->fields('n', array());

		$results = $query->execute();
	}
	
	$i = 1;
	$rows = array();
	foreach ($results as $col)
	{
			$rows[] = array(
							'data' => array(
								$i,
								$col->title,
								$col->field_book_author_value,
								$col->sku,
								$col->commerce_rent_item_location,
								
								l('Edit', $base_url.'/admin/commerce/rent/items/edit/'.$col->sku, array('html' => TRUE,)),
							)
						);
		$i++;
	}

	//Theme the html table
	$html .= theme('table', 
						array(
							'header' => $header,
							'rows'=>$rows,
							//'caption' => 'Creating Drupal 7 style tables',	//Optional Caption for the table
							'sticky' => TRUE,						//Optional to indicate whether the table headers should be sticky
							'empty' => 'There is no request...',		//Optional empty text for the table if result-set is empty
							)
						);

	//Append pager
	$html .= theme('pager',
				array(
					'tags' => array()
				)
			);
		
	return ($html);
}






//function which returns the form for update item
function commerce_rent_item_edit_item_form($form, &$form_state)
{
	if(empty($form_state['storage']['step']))
	{
    $form_state['storage']['step'] = 0;
  }
	
  switch($form_state['storage']['step'])
	{
    case 0:
      $form= commerce_rent_item_edit_item_form_1($form, $form_state);
    break;
    case 1:
      $form = commerce_rent_item_edit_item_form_2($form, $form_state);
    break;    
  }
  return $form;
}

//first form function for update details of an item
function commerce_rent_item_edit_item_form_1($form, $form_state)
{
	$form = array();
	
	//get sku
	$sku = arg(5);
	
	//get items details
	$get_item_details = get_edit_item_details($sku);
	
	//get product details
	$product = commerce_product_load_by_sku($sku);
	
  $form['item_sno'] = array(
      '#title' => t('Item SNo.'),
      '#type' => 'textfield',
      '#maxlength' => 128,
      '#value' => $get_item_details->commerce_rent_item_sno,
      '#attributes' => array(
				'disabled' => 'disabled',
				'style' => array(
					'border:none',
				),
      ),
    );
		
  $form['item_sku'] = array(
    '#title' => t('Item SKU/ISBN 13.'),
    '#type' => 'textfield',
		'#maxlength' => 128,
		'#value' => $sku,
		'#attributes' => array(
      'disabled' => 'disabled',
      'style' => array(
				'border:none',
      ),
    ),
  );
	
  $form['item_author'] = array(
    '#title' => t('Author.'),
    '#type' => 'textfield',
    '#maxlength' => 128,
    '#value' => $get_item_details->field_book_author_value,
    '#attributes' => array(
      'disabled' => 'disabled',
      'style' => array(
        'border:none',
      ),
    ),
  );
	
  $form['item_book'] = array(
    '#title' => t('Item.'),
    '#type' => 'textfield',
    '#maxlength' => 128,
    '#value' => $get_item_details->title,
    '#attributes' => array(
      'disabled' => 'disabled',
      'style' => array(
				'border:none',
			),
    ),
  );
	
	$variables = array(
    'path' =>isset($product->field_book_image['und'][0]['uri']) ? $product->field_book_image['und'][0]['uri'] : '',
    'width' => '35%',
    'height' => '35%',
    'alt' => isset($product->title) ? $product->title : '',
    'title' => isset($product->title) ? $product->title : '',
  );
	
  $img = theme('image', $variables);
	
	$form['item_img'] = array(
    '#title' => t('Image'),
    '#type' => 'item',
    '#markup' => isset($img) ? $img : '',
		'#prefix' => '<div style="width:40%; position:relative; float:right; top:-263px; right:-31px;">',
		'#suffix' => '</div>',
		'#attributes' => array(
      'style' => array(
				'width:35%',
				'height:35%',
			),
    ),
  );
	
	$location = isset($get_item_details->commerce_rent_item_location)? $get_item_details->commerce_rent_item_location : '';
	
	$form['item_location'] = array(
    '#title' => t('Location'),
    '#type' => 'textfield',
		'#default_value' => $location,
    '#maxlength' => 128,
    '#required' => TRUE,
  );
	
	$procurement_source = isset($get_item_details->commerce_rent_item_procurment_source)? $get_item_details->commerce_rent_item_procurment_source : '';
	
	$form['item_procurement_source'] = array(
		'#title' => t('Procurement Source'),
		'#type' => 'textfield',
		'#default_value' => $procurement_source,
		'#maxlength' => 128,
		'#required' => TRUE,
  );
	
	$procurement_price = isset($get_item_details->commerce_rent_item_procurment_price)? $get_item_details->commerce_rent_item_procurment_price : '';
	
	$form['item_procurement_price'] = array(
    '#title' => t('Procurement Price'),
    '#type' => 'textfield',
		'#default_value' => $procurement_price,
    '#maxlength' => 128,
    '#required' => TRUE,
  );
	
	$item_condition = isset($get_item_details->commerce_rent_item_condition)? $get_item_details->commerce_rent_item_condition : 'Select';
	
	$form['item_condition'] = array(
    '#title' => t('Condition'),
    '#type' => 'select',
    '#required' => TRUE,
    '#description'  => t('Select condition'),
		'#default_value' => $item_condition,
    '#options' => array(
      'Old'  => 'Old',
      'New' => 'New',
    ),
  );
	
	$form['item_cancel'] = array(
		'#type' => 'submit',
		'#value' => 'Cancel',
  );
	
	$form['item_submit'] = array(
		'#type' => 'submit',
		'#value' => 'Next',
  );
  
	return $form;
}

//second form function form update confirmation
function commerce_rent_item_edit_item_form_2($form, $form_state)
{
	$item = $form_state['storage'];
	
	//get product details
	$product = commerce_product_load_by_sku($item['item_sku']);
	
	$form = array();
	
	$form['item_sno'] = array(
		'#type' => 'item',
		'#title' => t('Item Sno'),
		'#markup' => $item['item_sno'],
  );
	
	$form['item_sku'] = array(
		'#type' => 'item',
		'#title' => t('Item SKU'),
		'#markup' => $item['item_sku'],
  );
	
	$form['item_author'] = array(
		'#type' => 'item',
		'#title' => t('Author Name'),
		'#markup' => $item['item_author'],
  );
	
	$form['item_book'] = array(
		'#type' => 'item',
		'#title' => t('Title'),
		'#markup' => $item['item_book'],
  );
	
	$variables = array(
    'path' =>isset($product->field_book_image['und'][0]['uri']) ? $product->field_book_image['und'][0]['uri'] : '',
    'width' => '35%',
    'height' => '35%',
    'alt' => isset($product->title) ? $product->title : '',
    'title' => isset($product->title) ? $product->title : '',
  );
	
  $img = theme('image', $variables);
	
	$form['item_img'] = array(
    '#title' => t('Image'),
    '#type' => 'item',
    '#markup' => isset($img) ? $img : '',
		'#prefix' => '<div style="width:40%; position:relative; float:right; top:-263px; right:-31px;">',
		'#suffix' => '</div>',
		'#attributes' => array(
      'style' => array(
				'width:35%',
				'height:35%',
			),
    ),
  );
	
	$form['item_location'] = array(
		'#type' => 'item',
		'#title' => t('Item Location'),
		'#markup' => $item['item_location'],
  );
	
	$form['item_procurement_source'] = array(
		'#type' => 'item',
		'#title' => t('Item Procurement Source'),
		'#markup' => $item['item_procurement_source'],
  );
	
	$form['item_procurement_price'] = array(
		'#type' => 'item',
		'#title' => t('Item Procurement Price'),
		'#markup' => $item['item_procurement_price'],
  );
	
	$form['item_condition'] = array(
		'#type' => 'item',
		'#title' => t('Item Condition'),
		'#markup' => $item['item_condition'],
  );
	
	$form['item_back'] = array(
		'#type' => 'submit',
		'#value' => 'Back',
  );
	
	$form['item_submit'] = array(
		'#type' => 'submit',
		'#value' => 'Submit',
  );
	
	return $form;
}

//submit function for main form function
function commerce_rent_item_edit_item_form_submit($form, &$form_state)
{
	switch ($form_state['storage']['step'])
	{
    case 0:
      switch($form_state['values']['op'])
			{
        case 'Next' :
					$form_state['storage']['item_sno'] = $form_state['values']['item_sno'];
					$form_state['storage']['item_sku'] = $form_state['values']['item_sku'];
					$form_state['storage']['item_author'] = $form_state['values']['item_author'];
					$form_state['storage']['item_book'] = $form_state['values']['item_book'];
					$form_state['storage']['item_location'] = $form_state['values']['item_location'];
					$form_state['storage']['item_procurement_source'] = $form_state['values']['item_procurement_source'];
					$form_state['storage']['item_procurement_price'] = $form_state['values']['item_procurement_price'];
					$form_state['storage']['item_condition'] = $form_state['values']['item_condition'];
          
					$form_state['storage']['step'] = 1;
					$form_state['rebuild'] = TRUE;
				break;
				
				case 'Cancel':
					drupal_goto('admin/commerce/rent/items/edit');
				break;
      }
    break;
		
    case 1:
      switch($form_state['values']['op'])
			{
				case 'Back':
					$form_state['storage']['step'] = 0;
					$form_state['rebuild'] = TRUE;
        break;
				case 'Submit' :
				
					$update = db_update('commerce_rent')
						->fields(array(
						'commerce_rent_item_location' => $form_state['storage']['item_location'],
						'commerce_rent_item_procurment_source' => $form_state['storage']['item_procurement_source'],
						'commerce_rent_item_procurment_price' => $form_state['storage']['item_procurement_price'],
						'commerce_rent_item_condition' => $form_state['storage']['item_condition'],
						))
						->condition('sku', $form_state['storage']['item_sku'],'=')
						->execute();
					
					dsm(t("Item updated successfully."));
					drupal_goto('admin/commerce/rent/items/edit');
				break;
      }
    break;
  } 

}

//function return item details
function get_edit_item_details($sku)
{
	$item = db_select('commerce_rent', 'n')
		->fields('n', array())
		->condition('sku', $sku, '=')
		->execute()
		->fetchObject();
	
	return $item;
}

function _commerce_rent_edit_item_search_field()
{
	$user_result = drupal_get_form('_commerce_rent_edit_item_search_field_request');
	return drupal_render($user_result);
}

function _commerce_rent_edit_item_search_field_request($form, &$form_state)
{
	$form = array();
	$form['#method'] = array('GET'); 
	$form['sku'] = array(
		'#title' => t('Search Item'),
    '#type' => 'textfield',
		'#required' => TRUE,
		'#maxlength' => 128,
		'#description' => 'Please enter Item SKU.',
	);
	
	$form['search_submit'] = array(
		'#type' => 'submit',
		'#value' => 'Submit',
  );
	
	if(isset($_GET['op']))
	{
		$form['view_all'] = array(
		'#type' => 'link',
    '#title' => t('view all'),
    '#href' => 'admin/commerce/rent/items/edit',
		);
	}
	
	return $form;
}

