select match_id,sec_to_time(duration) time
from match_info
order by duration desc
limit 5;