select d.position,lpad(f.champion_name,20,' ') champion_name,d.maxkda kda from(
     select lpad(max(b.kda),20,' ') maxkda, b.position from(
         select (a.sumkills+a.sumassists)/a.sumdeaths kda,a.position from(
             select sum(S.kills) sumkills,sum(S.assists) sumassists,sum(S.deaths) sumdeaths,P.position,P.champion_id 
             from participant P,stat S 
             where P.player_id=S.player_id 
             group by P.position, P.champion_id 
             having sum(S.deaths)!=0) as a
             ) as b 
             group by b.position) as d,
             (select lpad((c.sumkills+c.sumassists)/c.sumdeaths,20,' ') kda,c.position,c.champion_id 
             from(select sum(S.kills) sumkills,sum(S.assists) sumassists,sum(S.deaths) sumdeaths,P.position,P.champion_id 
             from participant P,stat S 
             where P.player_id=S.player_id 
             group by P.position,P.champion_id 
             having sum(S.deaths)!=0) as c) as e,champ f 
             where d.maxkda=e.kda and e.position=d.position and e.champion_id=f.champion_id and(e.position like '%AR%' or e.position like '%UP%' or e.position like '%UN%' or e.position like 'TO%' or e.position like 'MI%');

