select substring_index(M.version,'.',2),sum(f.win_res) as win_res, sum(f.lose_res) as lose_res from(
    select if(e.win=1,1,0) as win_res,if(e.win=0,1,0) as lose_res, e.match_id
    from(
        select a.match_id,S.win
        from(
            select match_id,player_id,player
            from participant
            where champion_id in(
            select champion_id
            from champ
            where champion_name like 'LEE SIN'
            )
        ) as a,(
            select match_id,player_id,player
            from participant
            where champion_id in(
            select champion_id
            from champ
            where champion_name like 'TEEMO'
            )
        ) as b, stat S
        where a.match_id=b.match_id  and S.player_id=a.player_id and floor(a.player/6)=floor(b.player/6)
    ) as e
) as f, match_info M
where f.match_id=M.match_id
group by substring_index(M.version,'.',2);