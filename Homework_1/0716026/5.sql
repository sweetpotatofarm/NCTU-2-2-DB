select ( case when c.win=5 then 'win' else 'lose' end) as win_lose,
count(c.summary) as cnt
from(
    select A.match_id, sum(B.win) win, sum(B.longesttimespentliving) summary
    from participant A, stat B
    where A.player_id=B.player_id
    group by A.match_id, floor(A.player/6)
    having sum(B.longesttimespentliving)>=6000
) as c
group by c.win;
