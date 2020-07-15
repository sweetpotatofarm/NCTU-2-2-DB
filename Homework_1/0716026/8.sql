select champion_name
from champ
where champion_id not in(
    select B.champion_id
    from match_info M, teamban B
    where M.match_id=B.match_id and substring_index(M.version,'.',2)=7.7
) order by champion_name;
