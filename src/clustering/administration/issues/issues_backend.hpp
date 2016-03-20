// Copyright 2010-2014 RethinkDB, all rights reserved.
#ifndef CLUSTERING_ADMINISTRATION_ISSUES_ISSUES_BACKEND_HPP_
#define CLUSTERING_ADMINISTRATION_ISSUES_ISSUES_BACKEND_HPP_

#include <string>
#include <vector>

#include "errors.hpp"
#include <boost/shared_ptr.hpp>

#include "rdb_protocol/artificial_table/caching_cfeed_backend.hpp"
#include "clustering/administration/metadata.hpp"
#include "concurrency/watchable.hpp"
#include "clustering/administration/issues/local.hpp"
#include "clustering/administration/issues/name_collision.hpp"
#include "clustering/administration/issues/non_transitive.hpp"
#include "clustering/administration/issues/outdated_index.hpp"
#include "clustering/administration/issues/table.hpp"

class namespace_repo_t;

class issues_artificial_table_backend_t :
    public timer_cfeed_artificial_table_backend_t
{
public:
    issues_artificial_table_backend_t(
        mailbox_manager_t *mailbox_manager,
        boost::shared_ptr<semilattice_read_view_t<cluster_semilattice_metadata_t> >
            _cluster_sl_view,
        watchable_map_t<peer_id_t, cluster_directory_metadata_t> *directory_view,
        server_config_client_t *server_config_client,
        table_meta_client_t *table_meta_client,
        namespace_repo_t *namespace_repo,
        admin_identifier_format_t identifier_format);
    ~issues_artificial_table_backend_t();

    std::string get_primary_key_name();

    bool read_all_rows_as_vector(signal_t *interruptor,
                                 std::vector<ql::datum_t> *rows_out,
                                 admin_err_t *error_out);

    bool read_row(ql::datum_t primary_key,
                  signal_t *interruptor,
                  ql::datum_t *row_out,
                  admin_err_t *error_out);

    bool write_row(ql::datum_t primary_key,
                   bool pkey_was_autogenerated,
                   ql::datum_t *new_value_inout,
                   signal_t *interruptor,
                   admin_err_t *error_out);

private:
    std::vector<scoped_ptr_t<issue_t> > all_issues(signal_t *interruptor) const;

    admin_identifier_format_t identifier_format;

    boost::shared_ptr<semilattice_read_view_t<cluster_semilattice_metadata_t> >
        cluster_sl_view;

    server_config_client_t *server_config_client;
    table_meta_client_t *table_meta_client;

    std::set<issue_tracker_t *> trackers;

    // Global issues are tracked here, local issues are collected from other servers by
    // the local_issue_client_t.
    local_issue_client_t local_issue_client;
    name_collision_issue_tracker_t name_collision_issue_tracker;
    table_issue_tracker_t table_issue_tracker;
    outdated_index_issue_tracker_t outdated_index_issue_tracker;
    non_transitive_issue_tracker_t non_transitive_issue_tracker;
};

#endif /* CLUSTERING_ADMINISTRATION_ISSUES_ISSUES_BACKEND_HPP_ */
