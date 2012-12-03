
-- уменьшение минимума игроков на отдельных БГ
UPDATE battleground_template SET MinPlayersPerTeam = 2 WHERE id IN (1,2,3,9,30,32,108,120);

