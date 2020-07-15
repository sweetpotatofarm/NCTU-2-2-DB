select c.champion_name, c.pentakills_ratio
from(
    select b.champion_name,b.pentakills/b.cnt pentakills_ratio
    from(
        select a.champion_name,sum(a.pentakills) pentakills,count(match_id) cnt
            from(
                select C.champion_name,S.pentakills, P.match_id
                from participant P, stat S, champ C
                where P.player_id=S.player_id and C.champion_id=P.champion_id
            ) as a
        group by a.champion_name
    ) as b
) as c
order by pentakills_ratio desc
limit 5;
