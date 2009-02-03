#include "replication_stub.h"
#include "l7vsd_stub.h"

namespace	l7vs{
// l7vsd
bool	l7vsd::list_vs_called(false);
bool	l7vsd::list_vs_v_called(false);
bool	l7vsd::add_vs_called(false);
bool	l7vsd::del_vs_called(false);
bool	l7vsd::edit_vs_called(false);
bool	l7vsd::add_rs_called(false);
bool	l7vsd::del_rs_called(false);
bool	l7vsd::edit_rs_called(false);
bool	l7vsd::flush_vs_called(false);
bool	l7vsd::rep_cmd_called(false);
bool	l7vsd::log_cmd_called(false);
bool	l7vsd::snm_cmd_called(false);
bool	l7vsd::param_called(false);

bool	l7vsd::list_vs_fail(false);
bool	l7vsd::list_vs_v_fail(false);
bool	l7vsd::add_vs_fail(false);
bool	l7vsd::del_vs_fail(false);
bool	l7vsd::edit_vs_fail(false);
bool	l7vsd::add_rs_fail(false);
bool	l7vsd::del_rs_fail(false);
bool	l7vsd::edit_rs_fail(false);
bool	l7vsd::flush_vs_fail(false);
bool	l7vsd::rep_cmd_fail(false);
bool	l7vsd::log_cmd_fail(false);
bool	l7vsd::snm_cmd_fail(false);
bool	l7vsd::param_fail(false);


}	//namespace	l7vs
