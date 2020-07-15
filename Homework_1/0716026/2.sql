select count(distinct substring_index(version,'.',2)) cnt
from match_info;