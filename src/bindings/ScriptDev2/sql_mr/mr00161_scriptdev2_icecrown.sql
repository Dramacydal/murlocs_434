DELETE FROM `script_texts` WHERE `entry` BETWEEN -1631594 AND -1631000;
INSERT INTO `script_texts` (`entry`,`content_loc8`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES

-- Lord Marrowgar
('-1631000','Это начало и конец, смертные. Никто не может войти в Храм!','This is the beginning AND the end, mortals. None may enter the master\'s sanctum!','16950','6','0','0','marrowgar SAY_INTRO'),
('-1631001','Проклятые несут миру смерть и разрушение!','The Scourge will wash over this world as a swarm of death and destruction!','16941','6','0','0','marrowgar SAY_AGGRO'),
('-1631002','Шторм костей!','BONE STORM!','16946','3','0','0','marrowgar SAY_BONESTORM'),
('-1631003','Проткнут костью!','Bound by bone!','16947','3','0','0','marrowgar SAY_BONESPIKE1'),
('-1631004','Кости вокруг!','Stick Around!','16948','3','0','0','marrowgar SAY_BONESPIKE2'),
('-1631005','Выход - только смерть!','The only escape is death!','16949','6','0','0','marrowgar SAY_BONESPIKE3'),
('-1631006','Больше костей!','More bones for the offering!','16942','6','0','0','marrowgar SAY_KILL1'),
('-1631007','Будьте прокляты!','Languish in damnation!','16943','6','0','0','marrowgar SAY_KILL2'),
('-1631008','ЯРОСТЬ МАСТЕРА ТЕЧЕТ ЧЕРЕЗ МЕНЯ!','THE MASTER\'S RAGE COURSES THROUGH ME!','16945','3','0','0','marrowgar SAY_ENRAGE'),
('-1631009','Я вижу... Только тьму...','I see... only darkness...','16944','6','0','0','marrowgar SAY_DEATH'),

-- Lady Deathwhisper
('-1631020','Взгляните на ваши мягкие руки! Сухожилия, мясо, кровь! Это слабость! Серьезная ошибка! Шутка создателя со своими творениями! Чем раньше вы поймете что жизнь - это дефект, тем раньше вы сможете преодолеть вашу слабость!','Fix your eyes upon your crude hands! The sinew, the soft meat, the dark blood coursing within! It is a weakness! A crippling flaw! A joke played by the creators upon their own creations! The sooner you come to accept your condition as a defect, the sooner you will find yourselves in a position to transcend it!','16878','6','0','0','deathwhisper SAY_INTRO1'),
('-1631021','Через нашего Мастера все возможно! Его сила не имеет предела, и его воля непреклонна! Те, кто против него будут истреблены! А те, кто служат, которые подчиняются полностью, беспрекословно, с беззаветной преданностью ума и души? Возвышены!','Through our master all things are possible! His power is without limit, and his will unbending! Those who oppose him will be destroyed utterly! And those who serve, who serve wholly, unquestioningly, with utter devotion of mind and soul? Elevated! To heights beyond your ken!','16879','6','0','0','deathwhisper SAY_INTRO2'),
('-1631022','Вы нашли свой путь здесь, потому что вы принадлежите к числу немногих одаренных истинным видением мира, проклятого слепотой! Вы можете видеть сквозь туман, что висит над этим миром, как саван, и понять, где истинная сила лжи!','You have found your way here, because you are among the few gifted with true vision in a world cursed with blindness! You can see through the fog that hangs over this world like a shroud and grasp where true power lies!','16880','6','0','0','deathwhisper SAY_INTRO3'),
('-1631023','Что это за беспорядок?! Вы смеете гадить на этой священной земле? Вот вам и место последнего упокоения!','What is this disturbance?! You dare trespass upon this hallowed ground? This shall be your final resting place.','16868','6','0','0','deathwhisper SAY_AGGRO'),
('-1631024','Однако! Я вижу что пора взять дело в свои руки.','Enough! I see I must take matters into my own hands!','16877','6','0','0','deathwhisper SAY_PHASE2'),
('-1631025','Вы слабы и бессильны против меня!','You are weak, powerless to resist my will!','16876','6','0','0','deathwhisper SAY_DOMINATEMIND'),
('-1631026','Возьмите это благословение и покажите этим злоумышленникам где раки зимуют!','Take this blessing and show these intruders a taste of our master\'s power.','16873','6','0','0','deathwhisper SAY_DARKEMPOWERMENT'),
('-1631027','Мои слуги! Я освобождаю вас от проклятия плоти!','Loyal adherent! I release you from the curse of flesh!','16874','6','0','0','deathwhisper SAY_DARKTRANSFORMATION'),
('-1631028','Встань и предстань в истинном виде!','Arise and exalt in your pure form!','16875','6','0','0','deathwhisper SAY_ANIMATEDEAD'),
('-1631029','Вы еще не осознали бесполезность своих действий?','Do you yet grasp of the futility of your actions?','16869','6','0','0','deathwhisper SAY_KILL1'),
('-1631030','Прими Тьму! Тьма вечна...','Embrace the darkness... Darkness eternal!','16870','6','0','0','deathwhisper SAY_KILL2'),
('-1631031','Это игра продолжается слишком долго!','This charade has gone on long enough.','16872','3','0','0','deathwhisper SAY_BERSERK'),
('-1631032','Все - части плана Мастера! Ваш конец неизбежен...','All part of the masters plan! Your end is... inevitable!','16871','6','0','0','deathwhisper SAY_DEATH'),

-- Saurfang
('-1631100','Во имя Короля-Лича!','BY THE MIGHT OF THE LICH KING!','16694','6','0','0','saurfang SAY_AGGRO'),
('-1631101','Земля обагрится вашей кровью!','The ground runs red with your blood!','16699','6','0','0','saurfang SAY_FALLENCHAMPION'),
('-1631102','Веселитесь, слуги мои!','Feast, my minions!','16700','3','0','0','saurfang SAY_BLOODBEASTS'),
('-1631103','Ты никто!','You are nothing!','16695','6','0','0','saurfang SAY_KILL1'),
('-1631104','Ваши души не найдут здесь избавления!','Your soul will find no redemption here!','16696','6','0','0','saurfang SAY_KILL2'),
('-1631105','Я вижу приближение смерти!','I have become... death!','16698','3','0','0','saurfang SAY_BERSERK'),
('-1631106','Я... Освободился...','I... Am... Released.','16697','6','0','0','saurfang SAY_DEATH'),
('-1631107','Все павшие воины орды, все дохлые псы альянса - все пополнят армию Короля-Лича! Даже сейчас валькиры воскрешают ваших покойников, чтобы они стали частью Плети!','For every Horde soldier that you killed, for every Alliance dog that fell, the Lich King\'s armies grew. Even now the Val\'kyr work to raise your fallen... As Scourge.','16701','6','0','0','saurfang SAY_INTRO'),
('-1631108','Сейчас все будет еще хуже. Идите сюда, я покажу вам, какой силой меня наделил Король-Лич!','Things are about to get much worse. Come, taste the power that the Lich King has bestowed upon me!','16702','6','0','0','saurfang SAY_BERSERK'),
('-1631109','Ха-ха-ха. Дворфы!','Hahahaha! Dwarves.','16703','6','0','0','saurfang SAY'),

-- Festergut
('-1631201','Отличные новости народ! Я починил трубы для подачи ядовитой смеси!','Good news, everyone! I\'ve fixed the poison slime pipes!','17123','6','0','0','Putricide Valve01'),
('-1631202','Тухлопуз! Ты всегда был моим любимчиком, как и Гниломорд... Молодец, что оставил столько газа. Я его даже чувствую.','Oh, Festergut. You were always my favorite. Next to Rotface. The good news is you left behind so much gas, I can practically taste it!','17124','6','0','0','Putricide Festergut Dead'),
('-1631203','Повеселимся!','Fun time!','16901','6','0','0','Festergut Aggro'),
('-1631204','Папочка! У меня получилось!','Daddy, I did it!','16902','6','0','0','Festergut Slay 01'),
('-1631205','Мертвец! Мертвец! Мертвец!','Dead, dead, dead!','16903','6','0','0','Festergut Slay 02'),
('-1631206','А-а-а-а-а...','Da ... Ddy...','16904','6','0','0','Festergut Death'),
('-1631207','Веселью конец!','Fun time over!','16905','6','0','0','Festergut Berserk'),
('-1631208','Что-то мне нехорошо...','Gyah! Uhhh, I not feel so good...','16906','6','0','0','Festergut Explunge Blight'),
('-1631209','Нет! Вы убили Вонючку! Сейчас получите!','NOOOO! You kill Stinky! You pay!','16907','6','0','0','Festergut Stinky Death '),
('-1631210','Их-ха!','...','16908','6','0','0','Festergut say '),
('-1631211','Ы-ы-ы!','...','16909','6','0','0','Festergut say '),
('-1631212','(Непереводимо)','...','16910','6','0','0','Festergut say '),
('-1631213','Пук.','...','16911','6','0','0','Festergut brrr '),

-- Rotface
('-1631220','Отличные новости народ! Слизь снова потекла!','Great news, everyone! The slime is flowing again!','17126','6','0','0','Putricide Slime Flow'),
('-1631221','Й-й-йя-хахаха!','WEEEEEE!','16986','6','0','0','Rotface Aggro'),
('-1631222','Я ЭТО заломал!','I brokes-ded it...','16987','6','0','0','Rotface Slay 01'),
('-1631223','Папочка сделает новые игрушки из тебя!','Daddy make toys out of you!','16988','6','0','0','Rotface Slay 02'),
('-1631224','Папочка, не огорчайся...','Bad news daddy.','16989','6','0','0','Rotface Death'),
('-1631225','Тихий час!','Sleepy Time!','16990','6','0','0','Rotface Berserk'),
('-1631226','Липучка-вонючка!','Icky sticky.','16991','6','0','0','Rotface Infection'),
('-1631227','Я сделал очень злую каку! Сейчас взорвется!','I think I made an angry poo-poo. It gonna blow!','16992','6','0','0','Rotface Unstable Ooze'),
('-1631228','Что? Прелесть? Не-е-е-т!','What? Precious? Noooooooooo!!!','16993','6','0','0','Rotface Precious played when precious dies'),
('-1631229','Й-а-а-а!','...','16994','6','0','0','Rotface say'),
('-1631230','Ах!','...','16995','6','0','0','Rotface say'),
('-1631231','А-а-у!','...','16996','6','0','0','Rotface say'),

-- Professor Putricide
('-1631240','Отличные новости народ! Я усовершенствовал штамм чумы, которая уничтожит весь Азерот!','Good news, everyone! I think I perfected a plague that will destroy all life on Azeroth!','17114','6','0','0','Putricide Aggro'),
('-1631241','М-м-м. Интересно.','Hmm... Interesting...','17115','6','0','0','Putricide Slay 01'),
('-1631242','О, как неожиданно!','That was unexpected!','17116','6','0','0','Putricide Slay 02'),
('-1631243','Плохие новости, народ... Похоже, у меня ничего не выйдет.','Bad news, everyone! I don\'t think I\'m going to make it.','17117','6','0','0','Putricide Death'),
('-1631244','Прекрасные новости, народ!','Great news, everyone!','17118','6','0','0','Putricide Berserk'),
('-1631245','Это обычное облако газа. Но будьте осторожны, не такое уж оно и обычное...','Just an ordinary gas cloud. But watch out, because that\'s no ordinary gas cloud!','17119','6','0','0','Putricide Gas Explosion'),
('-1631246','Что-то я ничего не чувствую. Что? Это еще откуда?','Hmm. I don\'t feel a thing. Whaa...? Where\'d those come from?','17120','6','0','0','Putricide Transform 01'),
('-1631247','На вкус как вишенка. Ой, извините...','Tastes like... Cherry! Oh! Excuse me!','17121','6','0','0','Putricide Transform 02'),
('-1631248','Два слизнюка в одной комнате? Может получиться что-то любопытное...','Two oozes, one room! So many delightful possibilities...','17122','6','0','0','Putricide Summon Ooze'),
('-1631249','Вы слишком грязные чтобы тут расхаживать! Надо сперва соскрести эту мерзкую плоть.','You can\'t come in here all dirty like that! You need that nasty flesh scrubbed off first!','17125','6','0','0','Putricide Airlock01 Before fight'),

-- Blood Prince Council
('-1631301','Глупые смертные! Думали, что одолели нас? Санлейн, непобедимые воины Короля-Лича! Теперь наши силы едины!','Foolish mortals. You thought us defeated so easily? The San\'layn are the Lich King\'s immortal soldiers! Now you shall face their might combined!','16795','6','0','0','Lanathel Intro Princes'),
('-1631302','Кушать подано!','Dinner... is served.','16681','6','0','0','Valanar Slay 01'),
('-1631303','Теперь вы видите, насколько мы сильны?','Do you see NOW the power of the Darkfallen?','16682','6','0','0','Valanar Slay 02'),
('-1631304','Охохо...','...why...?','16683','6','0','0','Valanar Death'),
('-1631305','Хорош тянуть время перед Санлейн!','BOW DOWN BEFORE THE SAN\'LAYN!','16684','6','0','0','Valanar Berserk'),
('-1631306','Наксанар был досадным недоразумением! Силы сферы позволят Валанару свершить отмщение!','Naxxanar was merely a setback! With the power of the orb, Valanar will have his vengeance!','16685','6','0','0','Valanar Empower'),
('-1631307','Моя чаша полна','My cup runneth over.','16686','6','0','0','Valanar Special'),
('-1631308','Йих!','...','16687','6','0','0','Princes say'),
('-1631309','Э-эх!','...','16688','6','0','0','Princes say'),
('-1631310','До-хо!','...','16689','6','0','0','Princes say'),
('-1631311','Восстаньте, братья, и уничтожьте наших врагов!','Arise, brothers, and destroy our enemies!','16796','6','0','0','Lanathel resurrect Princes'),

-- Blood Queen Lana'thel
('-1631321','Это было неразумное решение!','You have made an... unwise... decision.','16782','6','0','0','Lanathel Aggro'),
('-1631322','Я только попробую на вкус...','Just a taste...','16783','6','0','0','Lanathel Bite 01'),
('-1631323','Я голодна!','Know my hunger!','16784','6','0','0','Lanathel Bite 02'),
('-1631324','Смерть вас не спасет!','Death is no escape!','16785','6','0','0','Lanathel Add'),
('-1631325','Страдайте же!','SUFFER!','16786','6','0','0','Lanathel Special 01'),
('-1631326','Как вам такое?','Can you handle this?','16787','6','0','0','Lanathel Special 02'),
('-1631327','Начинаем представление!','Here it comes.','16788','6','0','0','Lanathel Special 03'),
('-1631328','Не повезло...','How... Unfortunate...','16789','6','0','0','Lanathel Reset'),
('-1631329','Нет. Моя прелесть, приятного аппетита!','Yes... feed my precious one! You\'re mine now!','16790','6','0','0','Lanathel Mind Control'),
('-1631330','Вот как... У тебя не получилось?','Really...? Is that all you\'ve got?','16791','6','0','0','Lanathel Slay 01'),
('-1631331','Какая жалость...','Such a pity!','16792','6','0','0','Lanathel Slay 02'),
('-1631332','Сейчас все кончится!','THIS! ENDS! NOW!','16793','6','0','0','Lanathel Berserk'),
('-1631333','Но... Мы ведь так хорошо ладили...','But... we were getting along... so well...','16794','6','0','0','Lanathel Death'),

-- Valithria Dreamwalker
('-1631401','Герои! Вы должны мне помочь! Мои силы на исходе... Залечите мои раны...','Heroes, lend me your aid! I... I cannot hold them off much longer! You must heal my wounds!','17064','6','0','0','Valithria Aggro'),
('-1631402','Одержимые не знают отдыха...','No rest for the wicked...','17065','6','0','0','Valithria Slay Bad Hostile NPC'),
('-1631403','Прискобная потеря.','A tragic loss...','17066','6','0','0','Valithria Slay Good - Player'),
('-1631404','Неудачники!','FAILURES!','17067','6','0','0','Valithria Berserk'),
('-1631405','Я открыла портал в изумрудный сон. Там вы найдете спасение, герои!','I have opened a portal into the Dream. Your salvation lies within, heroes.','17068','6','0','0','Valithria Dream World Open'),
('-1631406','Я долго не продержусь!','I will not last much longer!','17069','6','0','0','Valithria Health Low'),
('-1631407','Силы возвращаются ко мне! Герои, еще немного!','My strength is returning! Press on, heroes!','17070','6','0','0','Valithria Health High'),
('-1631408','Я излечилась! Изера, даруй мне силу покончить с этими нечестивыми тварями!','I am renewed! Ysera grants me the favor to lay these foul creatures to rest!','17071','6','0','0','Valithria Win'),
('-1631409','Простите меня, я не могу остано... ВСЕ ВО ВЛАСТИ КОШМАРА!','Forgive me for what I do! I... cannot... stop... ONLY NIGHTMARES REMAIN!','17072','6','0','0','Valithria Lose'),

-- Sindragosa
('-1631420','Глупцы! Зачем вы сюда явились? Ледяные ветра Нордскола унесут ваши души!','You are fools who have come to this place! The icy winds of Northrend will consume your souls!','17007','6','0','0','Sindragosa Aggro'),
('-1631421','Погибни!','Perish!','17008','6','0','0','Sindragosa Slay 01'),
('-1631422','Удел смертных!','A flaw of mortality...','17009','6','0','0','Sindragosa Slay 02'),
('-1631423','Наконец-то! Свободна!','Free...at last...','17010','6','0','0','Sindragosa Death'),
('-1631424','Хватит! Эти игры меня утомляют!','Enough! I tire of these games!','17011','6','0','0','Sindragosa Berserk'),
('-1631425','Здесь ваше вторжение и окончится! Никто не уцелеет!','Your incursion ends here! None shall survive!','17012','6','0','0','Sindragosa Take Off - fly'),
('-1631426','Вы чувствуете, как ледяная ладонь смерти сжимает сердце?','Can you feel the cold hand of death upon your heart?','17013','6','0','0','Sindragosa Freeze'),
('-1631427','Трепещите, смертные! Ибо ваша жалкая магия теперь бессильна!','Suffer, mortals, as your pathetic magic betrays you!','17014','6','0','0','Sindragosa Arcane'),
('-1631428','А-а-а! Жжот! Что это за колдовство?','Suffer, mortals, as your pathetic magic betrays you!','17015','6','0','0','Sindragosa Special'),
('-1631429','А теперь почувствуйте всю мощь Господина и погрузитесь в отчаяние!','Now feel my master\'s limitless power and despair!','17016','6','0','0','Sindragosa Low HP'),

-- Lich king
('-1631501','Неужели прибыли, наконец, хваленые силы света? Мне бросить Ледяную скорбь и сдаться на твою милость, Фордринг?','So...the Light\'s vaunted justice has finally arrived. Shall I lay down Frostmourne and throw myself at your mercy, Fordring?','17349','6','0','0','Lich King SAY_INTRO1'),
('-1631503','Ты пройдешь через эти мучения сам.','You will learn of that first hand. When my work is complete, you will beg for mercy -- and I will deny you. Your anguished cries will be testament to my unbridled power.','17350','6','0','0','Lich King SAY_INTRO3'),
('-1631505','Я оставлю тебя в живых, чтобы ты увидел финал! Не могу допустить чтобы величайший служитель света пропустил рождение МОЕГО мира!','I\'ll keep you alive to witness the end, Fordring. I would not want the Light\'s greatest champion to miss seeing this wretched world remade in my image.','17351','6','0','0','Lich King SAY_AGGRO'),
('-1631506','Ну же, герои! В вашей ярости - МОЯ сила!','Come then champions, feed me your rage!','17352','6','0','0','Lich King SAY'),
('-1631507','Сомнений нет - вы сильнейшие герои Азерота! Вы преодолели все препятствия, которые я воздвиг перед вами! Сильнейшие из моих слуг пали под вашим натиском, сгорели в пламени вашей ярости!','No question remains unanswered. No doubts linger. You are Azeroth\'s greatest champions! You overcame every challenge I laid before you. My mightiest servants have fallen before your relentless onslaught, your unbridled fury..','17353','6','0','0','Lich King SAY'),
('-1631508','Что движет вами? Праведность? Не знаю...','Is it truly righteousness that drives you? I wonder.','17354','6','0','0','Lich King SAY'),
('-1631509','Ты отлично их обучил, Фордринг! Ты привел сюда лучших воинов, которых знал мир! И отдал их в мои руки. Как я и рассчитывал.','You trained them well, Fordring. You delivered the greatest fighting force this world has ever known... right into my hands -- exactly as I intended. You shall be rewarded for your unwitting sacrifice.','17355','6','0','0','Lich King SAY'),
('-1631510','Смотри как я буду воскрешать их и превращать в воинов Плети! Они повергнут этот мир в пучину хаоса. Азерот падет от их рук. И ты станешь первой жертвой. ','Watch now as I raise them from the dead to become masters of the Scourge. They will shroud this world in chaos and destruction. Azeroth\'s fall will come at their hands -- and you will be the first to die.','17356','6','0','0','Lich King SAY'),
('-1631511','Мне по душе эта ирония!','I delight in the irony.','17357','6','0','0','Lich King SAY'),
('-1631512','Невозможно!','Impossible...','17358','6','0','0','Lich King SAY'),
('-1631513','Да! Вы меня и правда ранили. Я слишком долго с вами играл. Испытайте на себе возмездие Смерти!','You gnats actually hurt me! Perhaps I\'ve toyed with you long enough, now taste the vengeance of the grave!','17359','6','0','0','Lich King SAY'),
('-1631514','А-а-х!','...','17360','6','0','0','Lich King SAY'),
('-1631515','И вот я стою как лев пред агнцами. И не дрожат они.','Now I stand, the lion before the lambs... and they do not fear.','17361','6','0','0','Lich King SAY'),
('-1631516','Им неведом страх!','They cannot fear.','17362','6','0','0','Lich King SAY'),
('-1631517','Надежда тает!','Hope wanes!','17363','6','0','0','Lich King SAY'),
('-1631518','Пришел конец!','The end has come!','17364','6','0','0','Lich King SAY'),
('-1631519','Встречайте трагический финал!','Face now your tragic end!','17365','6','0','0','Lich King SAY_KILL'),
('-1631520','Ледяная скорбь жаждет крови!','Frostmourne hungers...','17366','6','0','0','Lich King SAY'),
('-1631521','Ледяная скорбь, повинуйся мне!','Argh... Frostmourne, obey me!','17367','6','0','0','Lich King SAY'),
('-1631522','Ледяная скорбь поглотит душу вашего товарища!','Frostmourne feeds on the soul of your fallen ally!','17368','6','0','0','Lich King SAY_KILL'),
('-1631523','Я проморожу вас насквозь и вы разлетитесь на ледяные осколки!','I will freeze you from within until all that remains is an icy husk!','17369','6','0','0','Lich King SAY'),
('-1631524','Смотрите, как мир рушится вокруг вас!','Watch as the world around you collapses!','17370','6','0','0','Lich King SAY_WIN'),
('-1631525','Конец света!','Apocalypse!','17371','6','0','0','Lich King SAY'),
('-1631526','Склонись перед своим господином и повелителем!','Bow down before your lord and master!','17372','6','0','0','Lich King SAY'),
('-1631527','Валькирия! Твой господин зовет!','Val\'kyr, your master calls!','17373','6','0','0','Lich King SAY_SUMMON'),
('-1631528','...','...','17374','6','0','0','Lich King SAY_DEATH'),
('-1631531','Оскверняю!','Defile!','0','3','0','0','Lich King SAY'),

-- Tirion
('-1631552','Мы даруем тебе быструю смерть, Артас! Более быструю чем ты заслуживаешь за то что замучил и погубил десятки тысяч жизней!','We will grant you a swift death, Arthas. More than can be said for the thousands you\'ve tortured and slain.','17390','6','0','0','Tirion SAY_INTRO2'),
('-1631554','Да будет так! Герои, в атаку','So be it. Champions, attack!','17391','6','0','0','Tirion SAY_INTRO4'),
('-1631555','Свет! Даруй мне последнее благословение! Дай мне разбить эти оковы!','LIGHT, GRANT ME ONE FINAL BLESSING. GIVE ME THE STRENGTH... TO SHATTER THESE BONDS!','17392','6','0','0','Tirion SAY'),
('-1631556','Хватит, Артас! Твоя ненависть не заберет больше ни одной жизни!','No more, Arthas! No more lives will be consumed by your hatred!','17393','6','0','0','Tirion SAY'),

-- Menethil
('-1631557','Вы пришли чтобы вершить суд над Артасом? Чтобы уничтожить короля-лича?','You have come to bring Arthas to justice? To see the Lich King destroyed?','17394','6','0','0','Terenas Menethil II SAY'),
('-1631558','Вы не должны оказаться во власти Ледяной скорби. Иначе, как и я, будете навеки порабощены этим проклятым клинком.','First, you must escape Frostmourne\'s hold, or be damned as I am; trapped within this cursed blade for all eternity.','17395','6','0','0','Terenas Menethil II SAY'),
('-1631559','Помогите мне уничтожить эти истерзанные души. Вместе мы вытянем силу из ледяной скорби и ослабим короля-лича.','Aid me in destroying these tortured souls! Together we will loosen Frostmourne\'s hold and weaken the Lich King from within!','17396','6','0','0','Terenas Menethil II SAY'),
('-1631560','Наконец я свободен. Все кончено, сын мой. Настал час расплаты.','Free at last! It is over, my son. This is the moment of reckoning.','17397','6','0','0','Terenas Menethil II SAY'),
('-1631561','Поднимитесь, воины света!','Rise up, champions of the Light!','17398','6','0','0',' SAY'),

-- Adds
('-1631590','Р-р-р-рота, подъем!','P-rr-company, the rise of','0','6','0','0','custom message'),
('-1631591','Хилы, не спать!','Heals, do not sleep','0','6','0','0','custom message'),
('-1631592','ДД поднажали!','DD put pressure','0','6','0','0','custom message'),
('-1631593','Лидер, гони лентяев из рейда! А то еще час возиться будете!','Leader, get lazy from a raid! And then another hour to tinker will','0','6','0','0','custom message'),
('-1631594','Ну вот вы и прикончили Артаса. Теперь будем ждать Катаклизм.','Well, you finished off Arthas. Now we wait for the cataclysm','0','6','0','0','custom message');

-- Gossips
DELETE FROM `gossip_texts` WHERE `entry` BETWEEN -3631608 AND -3631600;
INSERT INTO `gossip_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `comment`) VALUES
('-3631600', "Light\'s Hammer", NULL, NULL, NULL, NULL, NULL, NULL, NULL, "Молот света", "IceCrown citadel teleporter text 1"),
('-3631601', "Oratory of the Damned", NULL, NULL, NULL, NULL, NULL, NULL, NULL, "Молельня проклятых", "IceCrown citadel teleporter text 2"),
('-3631602', "Rampart of Skulls", NULL, NULL, NULL, NULL, NULL, NULL, NULL, "Черепной вал", "IceCrown citadel teleporter text 3"),
('-3631603', "Deathbringer\'s Rise", NULL, NULL, NULL, NULL, NULL, NULL, NULL, "Подъем Смертоносного", "IceCrown citadel teleporter text 4"),
('-3631604', "Icecrown Citadel", NULL, NULL, NULL, NULL, NULL, NULL, NULL, "Цитадель Ледяной Короны", "IceCrown citadel teleporter text 5"),
('-3631605', "The Sanctum of Blood", NULL, NULL, NULL, NULL, NULL, NULL, NULL, "Святилище крови", "IceCrown citadel teleporter text 6"),
('-3631606', "Frost Queen\'s Lair", NULL, NULL, NULL, NULL, NULL, NULL, NULL, "Логово Королевы льда", "IceCrown citadel teleporter text 7"),
('-3631607', "Frozen Throne", NULL, NULL, NULL, NULL, NULL, NULL, NULL, "Ледяной трон", "IceCrown citadel teleporter text 8"),
('-3631608', "We are ready, Tirion!", NULL, NULL, NULL, NULL, NULL, NULL, NULL, "Всегда готовы, дедуля!", "IceCrown citadel Tirion gossip");

-- Icecrown citadel spelltable

-- Lord Marrowgar
DELETE FROM `boss_spell_table` WHERE `entry` = 36612;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(36612, 69055, 70814, 69055, 70814, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0), -- handled by dynamic timer in script
(36612, 69138, 0, 0, 0, 6000, 0, 0, 0, 12000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36612, 71580, 0, 0, 0, 6000, 0, 0, 0, 12000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36612, 69146, 0, 0, 0, 15000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36612, 69057, 0, 0, 0, 17000, 0, 0, 0, 27000, 0, 0, 0, 0, 0, 0, 6, 0, 0),
(36612, 69076, 0, 0, 0, 45000, 0, 0, 0, 60000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36612, 69075, 0, 0, 0, 2000, 0, 0, 0, 2000, 0, 0, 0, 5, 0, 0, 12, 0, 0),
(36612, 47008, 0, 0, 0, 600000, 0, 0, 0, 600000, 0, 0, 0, 0, 0, 0, 1, 0, 0);
-- summons
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `timerMin_N10`, `timerMax_N10`,  `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `CastType` ) VALUES
(36612, 38711, 20000, 40000, 1, 1, 2, 2, 1, 5, 0, 9),
(36612, 36672, 45000, 45000, 1, 1, 2, 2, 75, 100, 0, 9);
-- Cold flame
DELETE FROM `boss_spell_table` WHERE `entry` = 36672;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(36672, 69145, 0, 0, 0, 3000, 3000, 8000, 8000, 3000, 3000, 8000, 8000, 15, 15, 15, 15, 0, 0, 0, 1, 0, 0),
(36672, 69147, 0, 0, 0, 700, 0, 0, 0, 700, 0, 0, 0, 15000, 15000, 30000, 30000, 0, 0, 0, 1, 0, 0),
(36672, 69146, 0, 0, 0, 1000, 0, 0, 0, 1000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `timerMin_N10`, `timerMax_N10`,  `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `CastType` ) VALUES
(36672, 36672, 30000, 30000, 1, 1, 1, 1, 0, 0, 0, 11);
-- Bone spike
DELETE FROM `boss_spell_table` WHERE `entry` = 38711;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(38711, 46598, 0, 0, 0, 1000, 0, 0, 0, 1000, 0, 0, 0, 18, 0, 0),
(38711, 69065, 0, 0, 0, 1000, 0, 0, 0, 1000, 0, 0, 0, 6, 0, 0);

-- Lady Deathwhisper
DELETE FROM `boss_spell_table` WHERE `entry` = 36855;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(36855, 70842, 0, 0, 0, 15000, 0, 0, 0, 25000, 0, 0, 0, 1, 0, 0),
(36855, 47008, 0, 0, 0, 600000, 0, 0, 0, 600000, 0, 0, 0, 1, 0, 0),
(36855, 71254, 72008, 72008, 72504, 5000, 0, 0, 0, 8000, 0, 0, 0, 4, 0, 0),
(36855, 71420, 72501, 72007, 72502, 15000, 0, 0, 0, 25000, 0, 0, 0, 4, 0, 0),
(36855, 71001, 0, 0, 0, 15000, 0, 0, 0, 25000, 0, 0, 0, 4, 0, 0),
(36855, 71204, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 3, 0, 0),
(36855, 70901, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 6, 0, 0),
(36855, 71289, 0, 0, 0, 15000, 0, 0, 0, 25000, 0, 0, 0, 4, 0, 0),
(36855, 71494, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 1, 0, 0);
-- summons
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `timerMin_N10`, `timerMax_N10`,  `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `CastType` ) VALUES
(36855, 37890, 45000, 75000, 1, 1, 2, 2, 75, 100, 0, 11),
(36855, 37949, 45000, 75000, 1, 1, 2, 2, 75, 100, 0, 11),
(36855, 38010, 45000, 45000, 1, 1, 2, 2, 75, 100, 0, 9),
(36855, 38222, 8000,  15000, 1, 1, 1, 1, 75, 100, 0, 9),
(36855, 38009, 45000, 45000, 1, 1, 2, 2, 75, 100, 0, 9);
-- Vengeful shade
DELETE FROM `boss_spell_table` WHERE `entry` = 38222;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(38222, 71494, 0, 0, 0, 1000, 0, 0, 0, 1000, 0, 0, 0, 1, 0, 0),
(38222, 71544, 0, 0, 0, 1000, 0, 0, 0, 1000, 0, 0, 0, 3, 0, 0);
-- Adherent
DELETE FROM `boss_spell_table` WHERE `entry` = 37949;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(37949, 71129, 0, 0, 0, 15000, 0, 0, 0, 20000, 0, 0, 0, 3, 0, 0),
(37949, 70594, 0, 0, 0, 5000, 0, 0, 0, 10000, 0, 0, 0, 4, 0, 0),
(37949, 71254, 0, 0, 0, 5000, 0, 0, 0, 15000, 0, 0, 0, 4, 0, 0),
(37949, 70906, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 4, 0, 0),
(37949, 70903, 0, 0, 0, 1000, 0, 0, 0, 2000, 0, 0, 0, 1, 0, 0),
(37949, 71237, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 3, 0, 0),
(37949, 70768, 0, 0, 0, 5000, 0, 0, 0, 10000, 0, 0, 0, 4, 0, 0),
(37949, 41236, 0, 0, 0, 5000, 0, 0, 0, 10000, 0, 0, 0, 1, 0, 0),
(37949, 71234, 0, 0, 0, 5000, 0, 0, 0, 10000, 0, 0, 0, 4, 0, 0);
-- Fanatic
DELETE FROM `boss_spell_table` WHERE `entry` = 37890;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(37890, 70659, 0, 0, 0, 5000, 0, 0, 0, 10000, 0, 0, 0, 3, 0, 0),
(37890, 70670, 0, 0, 0, 5000, 0, 0, 0, 10000, 0, 0, 0, 3, 0, 0),
(37890, 70674, 0, 0, 0, 5000, 0, 0, 0, 10000, 0, 0, 0, 1, 0, 0);

-- Gunship battle
-- Frost wyrm
DELETE FROM `boss_spell_table` WHERE `entry` = 37230;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(37230, 70116, 0, 0, 0, 15000, 0, 0, 0, 25000, 0, 0, 0, 4, 0, 0),
(37230, 70362, 0, 0, 0, 20000, 0, 0, 0, 25000, 0, 0, 0, 4, 0, 0),
(37230, 70361, 0, 0, 0, 3000, 0, 0, 0, 5000, 0, 0, 0, 3, 0, 0),
(37230, 47008, 0, 0, 0, 180000, 0, 0, 0, 180000, 0, 0, 0, 1, 0, 0);

-- Rotted frost giant
DELETE FROM `boss_spell_table` WHERE `entry` IN (38490, 38494);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(38490, 64652, 0, 0, 0, 15000, 0, 0, 0, 27000, 0, 0, 0, 4, 0, 0),
(38490, 72865, 0, 0, 0, 30000, 0, 0, 0, 45000, 0, 0, 0, 4, 0, 0),
(38490, 47008, 0, 0, 0, 300000, 0, 0, 0, 300000, 0, 0, 0, 1, 0, 0);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(38494, 64652, 0, 0, 0, 15000, 0, 0, 0, 27000, 0, 0, 0, 4, 0, 0),
(38494, 72865, 0, 0, 0, 25000, 0, 0, 0, 40000, 0, 0, 0, 4, 0, 0),
(38494, 47008, 0, 0, 0, 300000, 0, 0, 0, 300000, 0, 0, 0, 1, 0, 0);

-- Deathbringer Saurfang
DELETE FROM `boss_spell_table` WHERE `entry` = 37813;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`,`data1`, `data2`, `data3`, `data4`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(37813, 72178, 0, 0, 0, 20000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37813, 72371, 0, 0, 0, 3000, 0, 0, 0, 5000, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37813, 72256, 0, 0, 0, 1000, 0, 0, 0, 1000, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37813, 72293, 0, 0, 0, 35000, 0, 0, 0, 45000, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0),
(37813, 72737, 0, 0, 0, 15000, 0, 0, 0, 27000, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37813, 72385, 0, 0, 0, 20000, 0, 0, 0, 40000, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37813, 72380, 0, 0, 0, 25000, 0, 0, 0, 45000, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37813, 72408, 0, 0, 0, 20000, 0, 0, 0, 40000, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37813, 72172, 0, 0, 0, 45000, 0, 0, 0, 45000, 0, 0, 0, 2, 5, 2, 5, 1, 0, 0),
(37813, 72173, 0, 0, 0, 45000, 0, 0, 0, 45000, 0, 0, 0, 2, 5, 2, 5, 1, 0, 0),
(37813, 72356, 0, 0, 0, 45000, 0, 0, 0, 45000, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37813, 72357, 0, 0, 0, 45000, 0, 0, 0, 45000, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37813, 72358, 0, 0, 0, 45000, 0, 0, 0, 45000, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37813, 72769, 0, 0, 0, 15000, 0, 0, 0, 27000, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37813, 72723, 0, 0, 0, 15000, 0, 0, 0, 27000, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37813, 72242, 0, 0, 0, 1000, 0, 0, 0, 1000, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37813, 47008, 0, 0, 0, 480000, 0, 0, 0, 480000, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0);
-- summons
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `timerMin_N10`, `timerMax_N10`,  `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `CastType` ) VALUES
(37813, 38508, 45000, 45000, 1, 1, 2, 2, 15, 25, 0, 9);
-- Blood beast
DELETE FROM `boss_spell_table` WHERE `entry` = 38508;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(38508, 72176, 0, 0, 0, 15000, 0, 0, 0, 27000, 0, 0, 0, 1, 0, 0),
(38508, 72723, 0, 0, 0, 15000, 0, 0, 0, 27000, 0, 0, 0, 1, 0, 0),
(38508, 72769, 0, 0, 0, 15000, 0, 0, 0, 27000, 0, 0, 0, 1, 0, 0),
(38508, 21150, 0, 0, 0, 15000, 0, 0, 0, 27000, 0, 0, 0, 1, 0, 0);


-- Festergut
DELETE FROM `boss_spell_table` WHERE `entry` = 36626;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
( 36626, 69157, 0, 0, 0, 20000, 0, 0, 0, 20000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
( 36626, 69162, 0, 0, 0, 20000, 0, 0, 0, 20000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
( 36626, 69164, 0, 0, 0, 20000, 0, 0, 0, 20000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
( 36626, 69126, 0, 0, 0, 20000, 0, 0, 0, 20000, 0, 0, 0, 0, 0, 0, 6, 0, 0),
( 36626, 69152, 0, 0, 0, 20000, 0, 0, 0, 20000, 0, 0, 0, 0, 0, 0, 6, 0, 0),
( 36626, 69154, 0, 0, 0, 20000, 0, 0, 0, 20000, 0, 0, 0, 0, 0, 0, 6, 0, 0),
( 36626, 69165, 0, 0, 0, 10000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
( 36626, 69195, 0, 0, 0, 20000, 0, 0, 0, 20000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
( 36626, 69278, 0, 0, 0, 20000, 0, 0, 0, 20000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
( 36626, 69279, 0, 0, 0, 20000, 0, 0, 0, 20000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
( 36626, 69290, 0, 0, 0, 20000, 0, 0, 0, 20000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
( 36626, 69291, 0, 0, 0, 20000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 6, 0, 1),
( 36626, 72219, 0, 0, 0, 15000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 3, 0, 0),
( 36626, 72227, 0, 0, 0, 20000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 6, 0, 1),
( 36626, 72272, 0, 0, 0, 20000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 6, 0, 0),
( 36626, 69244, 0, 0, 0, 20000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 6, 0, 0),
( 36626, 69248, 0, 0, 0, 20000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 6, 0, 0),
( 36626, 72287, 0, 0, 0, 20000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
( 36626, 71379, 0, 0, 0, 20000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 6, 0, 1),
( 36626, 47008, 0, 0, 0, 600000, 0, 0, 0, 600000, 0, 0, 0, 0, 0, 0, 1, 0, 0);
-- summons
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `timerMin_N10`, `timerMax_N10`,  `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `CastType` ) VALUES
(36626, 38548, 12000, 12000, 1, 1, 1, 1, 10, 20, 0, 11);

-- Rotface
DELETE FROM `boss_spell_table` WHERE `entry` = 36627;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`,`data1`, `data2`, `data3`, `data4`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(36627, 69508, 0, 0, 0, 15000, 0, 0, 0, 20000, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36627, 69674, 0, 0, 0, 1000, 0, 0, 0,  1000, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0),
(36627, 70003, 0, 0, 0, 15000, 0, 0, 0, 15000, 0, 0, 0, 1, 2, 1, 3, 6, 0, 0),
(36627, 69788, 0, 0, 0, 20000, 0, 0, 0, 40000, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36627, 69783, 0, 0, 0, 30000, 0, 0, 0, 45000, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0),
(36627, 47008, 0, 0, 0, 600000, 0, 0, 0, 600000, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36627, 69789, 0, 0, 0, 20000, 0, 0, 0, 40000, 0, 0, 0, 0, 0, 0, 0, 6, 0, 1);
-- summons
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `timerMin_N10`, `timerMax_N10`,  `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `CastType` ) VALUES
(36627, 36897, 12000, 12000, 1, 1, 1, 1, 1, 5, 0, 9),
(36627, 37986, 15000, 15000, 1, 1, 1, 1, 10, 20, 0, 11),
(36627, 37013, 15000, 15000, 1, 1, 1, 1, 0, 0, 0, 11);
-- Small ooze
DELETE FROM `boss_spell_table` WHERE `entry` = 36897;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(36897,69774, 0, 0, 0, 10000, 0, 0, 0, 15000, 0, 0, 0, 4, 0, 0),
(36897,69750, 0, 0, 0, 10000, 0, 0, 0, 15000, 0, 0, 0, 1, 0, 0),
(36897,69644, 0, 0, 0, 10000, 0, 0, 0, 15000, 0, 0, 0, 6, 0, 0),
(36897,69889, 0, 0, 0, 10000, 0, 0, 0, 15000, 0, 0, 0, 6, 0, 0);
-- summons
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `timerMin_N10`, `timerMax_N10`,  `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `CastType` ) VALUES
(36897, 36899, 12000, 12000, 1, 1, 1, 1, 1, 5, 0, 9);
-- Big ooze
DELETE FROM `boss_spell_table` WHERE `entry` = 36899;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(36899,69774, 0, 0, 0, 15000, 0, 0, 0, 25000, 0, 0, 0, 4, 0, 0),
(36899,69839, 0, 0, 0, 6000, 0, 0, 0, 6000, 0, 0, 0, 1, 0, 0),
(36899,69760, 0, 0, 0, 10000, 0, 0, 0, 15000, 0, 0, 0, 1, 0, 0),
(36899,69644, 0, 0, 0, 10000, 0, 0, 0, 15000, 0, 0, 0, 1, 0, 0),
(36899,69558, 0, 0, 0, 10000, 0, 0, 0, 15000, 0, 0, 0, 1, 0, 0),
(36899,69889, 0, 0, 0, 10000, 0, 0, 0, 15000, 0, 0, 0, 6, 0, 0);
-- Ooze explode stalker
DELETE FROM `boss_spell_table` WHERE `entry` = 38107;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(38107,69840, 0, 0, 0, 2000, 0, 0, 0, 2000, 0, 0, 0, 1, 0, 0);

-- Professor Putricide
DELETE FROM `boss_spell_table` WHERE `entry` = 36678;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(36678,70351, 0, 0, 0, 40000, 0, 0, 0, 60000, 0, 0, 0, 0, 1, 0, 0),
(36678,71617, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 0, 3, 0, 0),
(36678,71615, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 100, 12, 0, 0),
(36678,71618, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 100, 12, 0, 0),
(36678,71621, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 0, 3, 0, 0),
(36678,71278, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 0, 3, 0, 0),
(36678,71279, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 0, 3, 0, 0),
(36678,71893, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 0, 3, 0, 0),
(36678,71273, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 1, 0, 0),
(36678,71275, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 1, 0, 0),
(36678,71276, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 1, 0, 0),
(36678,71702, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 0, 1, 0, 0),
(36678,71703, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 0, 1, 0, 0),
(36678,71603, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 0, 3, 0, 0),
(36678,70311, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 0, 1, 0, 0),
(36678,47008, 0, 0, 0, 600000, 0, 0, 0, 600000, 0, 0, 0, 0, 1, 0, 0),
(36678,71518, 0, 0, 0, 1000, 0, 0, 0, 1000, 0, 0, 0, 0, 6, 0, 1),
(36678,72672, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 3, 0, 0);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(36678,70342, 0, 0, 0, 30000, 0, 0, 0, 60000, 0, 0, 0, 10, 40, 0, 15, 0, 0),
(36678,70852, 0, 0, 0, 15000, 0, 0, 0, 25000, 0, 0, 0, 10, 30, 0, 15, 0, 0);

-- summons
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `timerMin_N10`, `timerMax_N10`,  `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `CastType` ) VALUES
(36678, 37562, 20000, 40000, 1, 1, 1, 1, 5, 10, 0, 9),
(36678, 38159, 8000,  20000, 1, 1, 1, 1, 20, 40, 0, 9),
(36678, 37690, 40000, 60000, 1, 1, 1, 1, 10, 40, 0, 9),
(36678, 37697, 20000, 40000, 1, 1, 1, 1, 5, 10, 0, 9);
-- Gas cloud
DELETE FROM `boss_spell_table` WHERE `entry` = 37562;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(37562,70672, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 6, 0, 0),
(37562,70215, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 6, 0, 0),
(37562,71770, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 6, 0, 1),
(37562,70812, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 6, 0, 1),
(37562,70701, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 6, 0, 0);
-- Volatile ooze
DELETE FROM `boss_spell_table` WHERE `entry` = 37697;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(37697,70492, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 1, 0, 0),
(37697,70530, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 6, 0, 1),
(37697,71770, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 6, 0, 1),
(37697,70447, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 6, 0, 0);
-- Mutated abomination (pet?)
DELETE FROM `boss_spell_table` WHERE `entry` = 37672;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(37672,70311, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 1, 0, 0),
(37672,72527, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 3, 0, 0),
(37672,72539, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 3, 0, 0),
(37672,70542, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 3, 0, 0),
(37672,70405, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 1, 0, 0);

-- Taldaram
DELETE FROM `boss_spell_table` WHERE `entry` = 37973;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(37973, 71807, 0, 0, 0, 50000, 0, 0, 0, 10000, 0, 0, 0, 0, 0, 0, 3, 0, 0),
(37973, 71718, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37973, 71719, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 15, 0, 0),
(37973, 72040, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37973, 72041, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 15, 0, 0),
(37973, 70952, 0, 0, 0, 30000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37973, 70981, 0, 0, 0, 30000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37973, 70982, 0, 0, 0, 30000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37973, 70983, 0, 0, 0, 1000, 0, 0, 0, 1000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37973, 47008, 0, 0, 0, 600000, 0, 0, 0, 600000, 0, 0, 0, 0, 0, 0, 1, 0, 0);

-- Valanar
DELETE FROM `boss_spell_table` WHERE `entry` = 37970;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(37970, 72053, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37970, 38459, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 11, 0, 0),
(37970, 72037, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37970, 38422, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 11, 0, 0),
(37970, 71945, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 6, 0, 0),
(37970, 47008, 0, 0, 0, 600000, 0, 0, 0, 600000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37970, 70983, 0, 0, 0, 1000, 0, 0, 0, 1000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37970, 70981, 0, 0, 0, 30000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37970, 70982, 0, 0, 0, 30000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37970, 70952, 0, 0, 0, 30000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37970, 72039, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 4, 0, 0);

-- Keleseth
DELETE FROM `boss_spell_table` WHERE `entry` = 37972;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(37972, 71405, 0, 0, 0, 5000, 0, 0, 0, 8000, 0, 0, 0, 0, 0, 0, 3, 0, 0),
(37972, 71815, 0, 0, 0, 10000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 3, 0, 0),
(37972, 71943, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37972, 71822, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37972, 47008, 0, 0, 0, 600000, 0, 0, 0, 600000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37972, 70952, 0, 0, 0, 30000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37972, 70981, 0, 0, 0, 30000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37972, 70982, 0, 0, 0, 30000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37972, 70983, 0, 0, 0, 1000, 0, 0, 0, 1000, 0, 0, 0, 0, 0, 0, 1, 0, 0);

-- Lanathel
DELETE FROM `boss_spell_table` WHERE `entry` = 37955;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(37955, 72981, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37955, 71623, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 6, 0, 0),
(37955, 70451, 70451, 71510, 71510, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 6, 0, 1),
(37955, 70445, 70445, 70821, 70821, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 6, 0, 0),
(37955, 71726, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37955, 70867, 71473, 71532, 71533, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37955, 70871, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37955, 70923, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37955, 71340, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37955, 71264, 0, 0, 0, 20000, 0, 0, 0, 35000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37955, 71446, 0, 0, 0, 7000, 0, 0, 0, 12000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37955, 71772, 0, 0, 0, 40000, 0, 0, 0, 60000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37955, 47008, 0, 0, 0, 600000, 0, 0, 0, 600000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37955, 72934, 0, 0, 0, 1000, 0, 0, 0, 1000, 0, 0, 0, 0, 0, 0, 6, 0, 1),
(37955, 71952, 0, 0, 0, 5000, 0, 0, 0, 8000, 0, 0, 0, 0, 0, 0, 1, 0, 0);

-- Valithria
DELETE FROM `boss_spell_table` WHERE `entry` = 36789;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(36789, 71977, 0, 0, 0, 30000, 0, 0, 0, 60000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36789, 71987, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 1, 1, 0),
(36789, 72481, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36789, 70873, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36789, 71189, 0, 0, 0, 3000, 0, 0, 0, 3000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36789, 72724, 0, 0, 0, 3000, 0, 0, 0, 3000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36789, 70904, 0, 0, 0, 5000, 0, 0, 0, 5000, 0, 0, 0, 0, 0, 0, 1, 0, 1),
(36789, 71196, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36789, 70702, 0, 0, 0, 15000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 16, 0, 1);
-- summons
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `timerMin_N10`, `timerMax_N10`,  `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `CastType` ) VALUES
(36789, 38429, 30000, 40000, 1, 1, 1, 1, 5, 70, 0, 9),
(36789, 37868, 30000, 45000, 1, 1, 1, 1, 1, 3, 0, 11),
(36789, 37863, 30000, 45000, 1, 1, 1, 1, 1, 3, 0, 11),
(36789, 36791, 30000, 45000, 1, 1, 1, 1, 1, 3, 0, 11),
(36789, 37934, 30000, 45000, 1, 1, 1, 1, 1, 3, 0, 11),
(36789, 37886, 30000, 45000, 1, 1, 1, 1, 1, 3, 0, 11);
-- Nightmare portal
DELETE FROM `boss_spell_table` WHERE `entry` = 38429;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(38429, 70873, 0, 0, 0, 2000, 0, 0, 0, 2000, 0, 0, 0, 0, 0, 0, 1, 0, 1);

-- Sindragosa
DELETE FROM `boss_spell_table` WHERE `entry` = 36853;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(36853, 70084, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36853, 57764, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 1, 1, 0),
(36853, 19983, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 3, 0, 0),
(36853, 71077, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(36853, 69649, 0, 0, 0, 20000, 0, 0, 0, 35000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(36853, 70107, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(36853, 69762, 0, 0, 0, 10000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(36853, 69766, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 6, 0, 0),
(36853, 69846, 0, 0, 0, 15000, 0, 0, 0, 20000, 0, 0, 0, 50.0, 100.0, 0, 15, 0, 0),
(36853, 70117, 0, 0, 0, 30000, 0, 0, 0, 40000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36853, 70123, 0, 0, 0, 20000, 0, 0, 0, 35000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36853, 70126, 0, 0, 0, 90000, 0, 0, 0, 90000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(36853, 70157, 0, 0, 0, 6000, 0, 0, 0, 6000, 0, 0, 0, 0, 0, 0, 6, 0, 0),
(36853, 71665, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 6, 0, 0),
(36853, 69845, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(36853, 36980, 0, 0, 0, 90000, 0, 0, 0, 90000, 0, 0, 0, 0, 0, 0, 9, 0, 0),
(36853, 47008, 0, 0, 0, 600000, 0, 0, 0, 600000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36853, 72289, 0, 0, 0, 1000, 0, 0, 0, 1000, 0, 0, 0, 0, 0, 0, 12, 0, 1),
(36853, 70128, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 30.0, 0, 0, 12, 0, 0);

UPDATE`boss_spell_table` SET  `data1` = 2, `data2` = 5, `data3` = 2, `data4` =5 WHERE `entry` = 36853 AND `spellID_N10` = 70126;
UPDATE`boss_spell_table` SET  `data1` = 4, `data2` = 4, `data3` = 4, `data4` =4 WHERE `entry` = 36853 AND `spellID_N10` = 69845;

-- Rimefang
DELETE FROM `boss_spell_table` WHERE `entry` = 37533;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(37533, 71387, 0, 0, 0, 0, 0, 0, 0, 3600000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37533, 71386, 0, 0, 0, 5000, 0, 0, 0, 8000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37533, 71376, 0, 0, 0, 4000, 0, 0, 0, 8000, 0, 0, 0, 0, 0, 0, 4, 0, 0);
-- Spinestalker
DELETE FROM `boss_spell_table` WHERE `entry` = 37534;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(37534, 36922, 0, 0, 0, 8000, 0, 0, 0, 24000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37534, 40505, 0, 0, 0, 5000, 0, 0, 0, 8000, 0, 0, 0, 0, 0, 0, 3, 0, 0),
(37534, 71369, 0, 0, 0, 4000, 0, 0, 0, 8000, 0, 0, 0, 0, 0, 0, 4, 0, 0);
-- Ice tomb
DELETE FROM `boss_spell_table` WHERE `entry` = 36980;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(36980, 70157, 0, 0, 0, 2000, 0, 0, 0, 2000, 0, 0, 0, 0, 0, 0, 6, 0, 0);

-- Lich king
DELETE FROM `boss_spell_table` WHERE `entry` = 36597;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(36597, 70541, 0, 0, 0, 6000, 0, 0, 0, 12000, 0, 0, 0, 60, 0, 0, 12, 0, 0),
(36597, 70337, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(36597, 74074, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(36597, 69409, 0, 0, 0, 10000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 3, 0, 0),
(36597, 72762, 0, 0, 0, 20000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(36597, 68980, 0, 0, 0, 1500, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(36597, 68981, 0, 0, 0, 60000, 0, 0, 0, 60000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36597, 72133, 0, 0, 0, 5000, 0, 0, 0, 10000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36597, 72262, 0, 0, 0, 5000, 0, 0, 0, 5000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36597, 69201, 0, 0, 0, 5000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 4, 0, 1),
(36597, 69200, 0, 0, 0, 5000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(36597, 69242, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36597, 69103, 0, 0, 0, 3000, 0, 0, 0, 5000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36597, 69099, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36597, 69108, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36597, 70358, 0, 0, 0, 40000, 0, 0, 0, 70000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36597, 70372, 0, 0, 0, 40000, 0, 0, 0, 70000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36597, 72149, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36597, 72143, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36597, 70503, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36597, 69037, 0, 0, 0, 20000, 0, 0, 0, 40000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36597, 36609, 0, 0, 0, 20000, 0, 0, 0, 40000, 0, 0, 0, 0, 0, 0, 9, 0, 0),
(36597, 71769, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36597, 70063, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 1, 0, 1),
(36597, 47008, 0, 0, 0, 900000, 0, 0, 0, 900000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36597, 74352, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 1, 0, 0);
-- summons
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `timerMin_N10`, `timerMax_N10`,  `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `CastType` ) VALUES
(36597, 37799, 60000, 60000, 8, 10, 10, 10, 15, 25, 0, 11),
(36597, 70498, 3600001, 3600001, 12, 12, 15, 15, 15, 25, 0, 1);
-- ice sphere
DELETE FROM `boss_spell_table` WHERE `entry` = 36633;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(36633, 69099, 0, 0, 0, 2000, 0, 0, 0, 2000, 0, 0, 0, 0, 0, 0, 6, 0, 0),
(36633, 69108, 0, 0, 0, 2000, 0, 0, 0, 2000, 0, 0, 0, 0, 0, 0, 3, 0, 0),
(36633, 69090, 0, 0, 0, 8000, 0, 0, 0, 24000, 0, 0, 0, 0, 0, 0, 1, 0, 0);
-- vile spirit
DELETE FROM `boss_spell_table` WHERE `entry` = 37799;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(37799, 70503, 0, 0, 0, 10000, 0, 0, 0, 10000, 0, 0, 0, 0, 0, 0, 1, 0, 0);
-- raging spirit
DELETE FROM `boss_spell_table` WHERE `entry` = 36701;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(36701, 69242, 0, 0, 0, 2000, 0, 0, 0, 5000, 0, 0, 0, 0, 0, 0, 3, 0, 0);

