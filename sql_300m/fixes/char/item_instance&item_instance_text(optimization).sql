CREATE TABLE `item_instance_text` (
`guid` INT UNSIGNED  NOT NULL,
`text` LONGTEXT,
 PRIMARY KEY ( `guid` )
);

INSERT INTO item_instance_text SELECT guid, text FROM item_instance as it WHERE it.text != '';

ALTER TABLE item_instance DROP `text`;