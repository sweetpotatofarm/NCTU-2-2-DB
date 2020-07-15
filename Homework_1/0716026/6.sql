select e.position, f.champion_name from(
    select max(d.cnt) maxcount, d.position from(
        select count(B.player_id) cnt, B.position
        from match_info A, participant B, champ C
        where A.match_id=B.match_id and (A.duration between 2400 and 3000)
        and B.champion_id=C.champion_id
        group by B.position, B.champion_id
    ) as d group by d.position
)as e,(
    select count(B.player_id) cnt, B.position,C.champion_name
    from match_info A, participant B, champ C
    where A.match_id=B.match_id and (A.duration between 2400 and 3000)
    and B.champion_id=C.champion_id
    group by B.position, B.champion_id
)as f
where e.maxcount=f.cnt and e.position=f.position
and (f.position like '%AR%' or f.position like '%UP%' or f.position like '%NG%' or f.position like 'MI%' or f.position like 'TO%');