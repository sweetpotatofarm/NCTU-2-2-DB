select C.champion_name, count(P.champion_id) cnt
from champ C, participant P
where C.champion_id=P.champion_id and P.position like 'JUN%'
group by P.champion_id
order by count(P.champion_id) desc
limit 3;