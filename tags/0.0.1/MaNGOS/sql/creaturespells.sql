-- phpMyAdmin SQL Dump
-- version 2.6.2
-- http://www.phpmyadmin.net
-- 
-- Host: localhost
-- Generation Time: Sep 10, 2005 at 12:21 PM
-- Server version: 4.1.12
-- PHP Version: 5.0.4
-- 
-- Database: `mangos`
-- 

-- --------------------------------------------------------

-- 
-- Table structure for table `creaturespells`
-- 
-- Creation: Sep 10, 2005 at 12:02 PM
-- Last update: Sep 10, 2005 at 12:02 PM
-- 

DROP TABLE IF EXISTS `creaturespells`;
CREATE TABLE IF NOT EXISTS `creaturespells` (
  `entryid` int(11) NOT NULL default '0',
  `spellid` int(11) NOT NULL default '0',
  PRIMARY KEY  (`entryid`,`spellid`)
) TYPE=MyISAM;
