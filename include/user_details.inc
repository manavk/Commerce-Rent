<?php

//inc file which returns user details.

function user_full_name($uid)
{
	$user_fullname = db_select('field_data_field_user_first_name', 'f');
  $user_fullname->fields('f', array('field_user_first_name_value'));
	$user_fullname->condition('f.entity_id', $uid );
	$user_fullname->leftJoin('field_data_field_user_last_name', 'l', 'f.entity_id = l.entity_id');
  $user_fullname->fields('l', array('field_user_last_name_value'));
	$user_fullname->condition('l.entity_id', $uid );
  $user_name = $user_fullname->execute()->fetchAssoc();

	return $user_name;
}

function user_address($uid)
{
	$user_adress = db_select('field_data_field_user_address', 'c')
  ->fields('c', array('field_user_address_thoroughfare', 'field_user_address_premise', 'field_user_address_locality', 'field_user_address_postal_code', 'field_user_address_administrative_area'))
  ->condition('entity_id', $uid, '=')
  ->execute()->fetchAssoc();
	
	return $user_adress;
}

function user_mobile($uid)
{
	$user_mobile = db_select('field_data_field_user_mobile_number', 'c')
  ->fields('c', array('field_user_mobile_number_value'))
  ->condition('entity_id', $uid, '=')
  ->execute()->fetchAssoc();
	
	return $user_mobile;
}

function user_dob($uid)
{
	$user_dob = db_select('field_data_field_user_date_of_birth', 'c')
  ->fields('c', array('field_user_date_of_birth_value'))
  ->condition('entity_id', $uid, '=')
  ->execute()->fetchAssoc();
	
	return $user_dob;
}

function _user_mail($uid)
{
	$corporate_user_mail = db_select('users', 'c')
  ->fields('c', array('mail'))
  ->condition('uid', $uid, '=')
  ->execute()
	->fetchAssoc();
	
	return $corporate_user_mail;
}

function _user_no_of_delivery_eligible($uid)
{
	$user_no_of_delivery_eligible = db_select('field_data_field_no_of_delivery_eligible', 'c')
  ->fields('c', array('field_no_of_delivery_eligible_value'))
  ->condition('entity_id', $uid, '=')
  ->execute()
	->fetchObject();
	return $user_no_of_delivery_eligible;
}

function _user_subscription_validity($uid)
{
	$user_subscription_validity = db_select('field_data_field_commerce_sp_validity', 'n')
		->fields('n', array('field_commerce_sp_validity_value'))
		->condition('entity_id', $uid, '=')
		->execute()
		->fetchObject();
		
	return $user_subscription_validity;
}

function user_details_check($uid)
{
	$user_full_name = user_full_name($uid);
	$user_address = user_address($uid);
	$user_mobile = user_mobile($uid);
	$user_dob = user_dob($uid);
	
	if( !empty( $user_full_name['field_user_first_name_value'] ) && !empty( $user_full_name['field_user_last_name_value'] ) && !empty( $user_address['field_user_address_thoroughfare'] ) && !empty( $user_address['field_user_address_premise'] ) && !empty( $user_address['field_user_address_locality'] ) && !empty( $user_address['field_user_address_postal_code'] ) && !empty( $user_address['field_user_address_administrative_area'] ) && !empty( $user_mobile['field_user_mobile_number_value'] ) && !empty( $user_dob['field_user_date_of_birth_value'] ) )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

function _user_type($uid)
{
	$sub_query = db_select('users_roles', 'r')
  ->fields('r', array('rid'))
  ->condition('uid', $uid, '=')
	->execute()
	->fetchObject();

	$user_type = db_select('role', 'n')
  ->fields('n', array('name'))
  ->condition('rid', $sub_query->rid, '=')
	->execute()
	->fetchObject();

	return $user_type;
}

function _user_name($uid)
{
	$user_name = db_select('users', 'n')
		->fields('n', array('name'))
		->condition('uid', $uid, '=')
		->execute()
		->fetchObject();
	return $user_name;
}

function _get_user_uid($name)
{
	$user_uid = db_select('users', 'n')
		->fields('n', array('uid'))
		->condition('name', $name, '=')
		->execute()
		->fetchAssoc();
	return $user_uid;
}