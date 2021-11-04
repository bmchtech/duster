require 'duster'
require 'dusk'
require 'duster'

global function game_gs_ai_plan_moves(team_id: int32, move_buf: *QueuedMove, move_buf_len: int32): int32 <cexport, codename 'game_gs_ai_plan_moves'>
    return 1
end