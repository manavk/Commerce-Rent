<?php
//Main function which will call the orher form function 
function commerce_rent_form($form, &$form_state)
{
  if(empty($form_state['storage']['step']))
	{
    $form_state['storage']['step'] = 1;
  }
  switch($form_state['storage']['step'])
	{
    case 1:
      $form = commerce_rent_form_1($form, $form_state);
    break;
    case 2:
      $form = commerce_rent_form_2($form, $form_state);
    break;    
  }
  return $form;
}

//form function which will used for sku/isbn_13 submission.
function commerce_rent_form_1($form, $form_state)
{
  $form = array();
  $form['item_sku'] = array(
    '#title' => t('Item SKU/ISBN 13.'),
    '#type' => 'textfield',
    '#maxlength' => 128,
    '#required' => TRUE,
  );
  $form['item_sku_submit'] = array(
    '#type' => 'submit',
    '#value' => 'Next',
  );
  return $form;
}

//form function which will fetch the data according to sku/isbn_13 and show other field like procurement source, procurement price, condition etc.
function commerce_rent_form_2($form, $form_state){
  
	$form = array();
	
	//Save SKU
	$sku = $form_state['storage']['item_sku'];
	
	//get product details
	$product = commerce_product_load_by_sku($sku);
	
	//query for fetching the data from rent table.
  $q = db_select('commerce_rent', 's');
  $q->addExpression('MAX(commerce_rent_item_sno)');
	$q->condition('commerce_rent_item_sno', '%'.date('ymd').'%', 'LIKE');
	$max = $q->execute()->fetchAssoc();
  if(!empty($max['expression']))
	{
    $value = explode("LIB01IR".date('ymd'), $max['expression']);
    $item_sno = 'LIB01IR'.date('ymd').sprintf("%03d", ($value[1]+001));
  }
	else
	{
    $item_sno = 'LIB01IR'.date('ymd').'001';
  }
	
  $form['item_sno'] = array(
      '#title' => t('Item SNo.'),
      '#type' => 'textfield',
      '#maxlength' => 128,
      '#value' => $item_sno,
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
		'#value' => $form_state['storage']['item_sku'],
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
    '#value' => $product->field_author['und'][0]['value'],
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
    '#value' => $product->title,
    '#attributes' => array(
      'disabled' => 'disabled',
      'style' => array(
				'border:none',
			),
    ),
  );
	
	
	$variables = array(
    'path' => $product->field_book_image['und'][0]['uri'],
    'width' => '35%',
    'height' => '35%',
    'alt' => $product->title,
    'title' => $product->title,
  );
	
  $img = theme('image', $variables);
	
	$form['item_img'] = array(
    '#title' => t('Image'),
    '#type' => 'item',
    '#markup' => $img,
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
    '#title' => t('Location'),
    '#type' => 'textfield',
    '#maxlength' => 128,
    '#required' => TRUE,
  );
	
	$form['item_procurement_source'] = array(
		'#title' => t('Procurement Source'),
		'#type' => 'textfield',
		'#maxlength' => 128,
		'#required' => TRUE,
  );
	
	$form['item_procurement_price'] = array(
    '#title' => t('Procurement Price'),
    '#type' => 'textfield',
    '#maxlength' => 128,
    '#required' => TRUE,
  );
	
	$form['item_condition'] = array(
    '#title' => t('Condition'),
    '#type' => 'select',
    '#required' => TRUE,
    '#description'  => t('Select condition'),
    '#options' => array(
      'Old'  => 'Old',
      'New' => 'New',
    ),
  );
	
	$form['item_rent_sno'] = array(
		'#type' => 'hidden',
		'#value' => $item_sno,
	);
	
	$form['product_id'] = array(
		'#type' => 'hidden',
		'#value' => $product->product_id,
	);
	
	
	
	$form['item_rent_sku'] = array(
    '#type' => 'hidden',
    '#value' => $form_state['storage']['item_sku'],
  );
	
	$form['item_cancel'] = array(
		'#type' => 'submit',
		'#value' => 'Back',
  );
	
	$form['item_submit'] = array(
		'#type' => 'submit',
		'#value' => 'Submit',
  );
	
  return $form;
}

//submit function which will check the state of the form and call other function for data submission.
function commerce_rent_form_submit($form, &$form_state){
  switch ($form_state['storage']['step'])
	{
    case 1:
		
      switch($form_state['values']['op'])
			{
        case 'Next' :
				
          $product = commerce_product_load_by_sku($form_state['values']['item_sku']);
          
          if(!is_object($product))
					{
						drupal_set_message(t("No product exist for this SKU or you have entered a wrong SKU!"), "error");
            drupal_goto('admin/commerce/rent/items/add');
          }
					
					$form_state['storage']['item_sku'] = $form_state['values']['item_sku'];
					$form_state['storage']['step'] = 2;
          $form_state['rebuild'] = TRUE;
          
        break;
      }
    break;
		
    case 2:
      switch($form_state['values']['op'])
			{
				case 'Back':
					$form_state['storage']['step'] = 1;
					$form_state['rebuild'] = TRUE;
        break;
				
				case 'Submit' :
				
					$product = commerce_product_load_by_sku($form_state['values']['item_rent_sku']);
				
					$nid = db_insert('commerce_rent')
						->fields(array(
							'commerce_rent_item_sno' => $form_state['values']['item_rent_sno'],
							'product_id' => $form_state['values']['product_id'],
							'sku' => $form_state['values']['item_rent_sku'],
							'field_book_author_value' => $product->field_author['und'][0]['value'],
							'title' => $product->title,
							'commerce_rent_item_location' => $form_state['values']['item_location'],
							'commerce_rent_item_procurment_source' => $form_state['values']['item_procurement_source'],
							'commerce_rent_item_procurment_price' => $form_state['values']['item_procurement_price'],
							'commerce_rent_item_condition' => $form_state['values']['item_condition'],
							'filename' => $product->field_book_image['und'][0]['filename'],
							'field_isbn1book_3_value' => $product->sku,
							)
						)
						->execute();
					
					dsm(t("1 item added successfully"));
					drupal_goto('admin/commerce/rent/items/add');
					
				break;
      }
    break;
  }
}
