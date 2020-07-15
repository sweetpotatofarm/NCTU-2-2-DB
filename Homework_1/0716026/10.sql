select d.champion_name as self_champ_name,d.win_ratio,d.self_kda,d.self_avg_gold
,if(d.battle_record>=100,'Renekton','null') as enemy_champ_name,d.enemy_kda,d.enemy_avg_gold,d.battle_record
from(
    select c.champion_name,c.win_res/(c.win_res+c.lose_res) as win_ratio
    ,(c.kills+c.assists)/c.deaths as self_kda,c.goldearned/c.battle_record as self_avg_gold
    ,(c.e_kills+c.e_assists)/c.e_deaths as enemy_kda, c.e_goldearned/c.battle_record as enemy_avg_gold
    ,c.battle_record
    from(
        select b.champion_name,sum(b.win_res) win_res,sum(b.lose_res) lose_res
        ,sum(b.kills) kills,sum(b.assists) assists,sum(b.deaths) deaths,sum(b.goldearned) goldearned,count(b.match_id) record
        ,sum(b.e_kills) as e_kills, sum(b.e_assists) as e_assists, sum(b.e_deaths) as e_deaths, sum(b.e_goldearned) as e_goldearned
        ,count(b.match_id) as battle_record
        from(
            select a.match_id,C.champion_name,if(S.win=1,1,0) as win_res,if(S.win=0,1,0) as lose_res
            ,S.kills,S.deaths,S.assists,S.goldearned
            ,a.champion_name as e_champion_name,a.kills as e_kills,a.deaths as e_deaths, a.assists as e_assists, a.goldearned as e_goldearned
            from(
                select P.match_id, P.player, C.champion_name, S.kills, S.deaths, S.assists, S.goldearned
                from participant P, champ C, stat S
                where P.champion_id=C.champion_id and P.position like 'TO%' and C.champion_name like 'RENEKTON' and P.player_id=S.player_id
            )as a, stat S, participant P, champ C
            where S.player_id=P.player_id and floor(P.player/6)<>floor(a.player/6) 
            and a.match_id=P.match_id and C.champion_id=P.champion_id and P.position like 'TO%'
        ) as b
        group by b.champion_name
    ) as c
    where c.battle_record>=100
) as d
order by d.win_ratio desc
limit 5;


 
