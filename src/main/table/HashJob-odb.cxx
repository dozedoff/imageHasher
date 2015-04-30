// This file was generated by ODB, object-relational mapping (ORM)
// compiler for C++.
//

#include <odb/pre.hxx>

#include "HashJob-odb.hxx"

#include <cassert>
#include <cstring>  // std::memcpy

#include <odb/schema-catalog-impl.hxx>

#include <odb/sqlite/traits.hxx>
#include <odb/sqlite/database.hxx>
#include <odb/sqlite/transaction.hxx>
#include <odb/sqlite/connection.hxx>
#include <odb/sqlite/statement.hxx>
#include <odb/sqlite/statement-cache.hxx>
#include <odb/sqlite/simple-object-statements.hxx>
#include <odb/sqlite/container-statements.hxx>
#include <odb/sqlite/exceptions.hxx>
#include <odb/sqlite/prepared-query.hxx>
#include <odb/sqlite/simple-object-result.hxx>

namespace odb
{
  // HashJob
  //

  const char alias_traits<  ::imageHasher::db::table::ImageRecord,
    id_sqlite,
    access::object_traits_impl< ::imageHasher::db::table::HashJob, id_sqlite >::imageRecord_tag>::
  table_name[] = "\"imageRecord\"";

  struct access::object_traits_impl< ::imageHasher::db::table::HashJob, id_sqlite >::extra_statement_cache_type
  {
    extra_statement_cache_type (
      sqlite::connection&,
      image_type&,
      sqlite::binding&,
      sqlite::binding&)
    {
    }
  };

  access::object_traits_impl< ::imageHasher::db::table::HashJob, id_sqlite >::id_type
  access::object_traits_impl< ::imageHasher::db::table::HashJob, id_sqlite >::
  id (const image_type& i)
  {
    sqlite::database* db (0);
    ODB_POTENTIALLY_UNUSED (db);

    id_type id;
    {
      sqlite::value_traits<
          unsigned int,
          sqlite::id_integer >::set_value (
        id,
        i.job_id_value,
        i.job_id_null);
    }

    return id;
  }

  bool access::object_traits_impl< ::imageHasher::db::table::HashJob, id_sqlite >::
  grow (image_type& i,
        bool* t)
  {
    ODB_POTENTIALLY_UNUSED (i);
    ODB_POTENTIALLY_UNUSED (t);

    bool grew (false);

    // job_id
    //
    t[0UL] = false;

    // sha256
    //
    if (t[1UL])
    {
      i.sha256_value.capacity (i.sha256_size);
      grew = true;
    }

    // imageRecord
    //
    t[2UL] = false;

    return grew;
  }

  void access::object_traits_impl< ::imageHasher::db::table::HashJob, id_sqlite >::
  bind (sqlite::bind* b,
        image_type& i,
        sqlite::statement_kind sk)
  {
    ODB_POTENTIALLY_UNUSED (sk);

    using namespace sqlite;

    std::size_t n (0);

    // job_id
    //
    if (sk != statement_update)
    {
      b[n].type = sqlite::bind::integer;
      b[n].buffer = &i.job_id_value;
      b[n].is_null = &i.job_id_null;
      n++;
    }

    // sha256
    //
    b[n].type = sqlite::image_traits<
      ::std::string,
      sqlite::id_text>::bind_value;
    b[n].buffer = i.sha256_value.data ();
    b[n].size = &i.sha256_size;
    b[n].capacity = i.sha256_value.capacity ();
    b[n].is_null = &i.sha256_null;
    n++;

    // imageRecord
    //
    b[n].type = sqlite::bind::integer;
    b[n].buffer = &i.imageRecord_value;
    b[n].is_null = &i.imageRecord_null;
    n++;
  }

  void access::object_traits_impl< ::imageHasher::db::table::HashJob, id_sqlite >::
  bind (sqlite::bind* b, id_image_type& i)
  {
    std::size_t n (0);
    b[n].type = sqlite::bind::integer;
    b[n].buffer = &i.id_value;
    b[n].is_null = &i.id_null;
  }

  bool access::object_traits_impl< ::imageHasher::db::table::HashJob, id_sqlite >::
  init (image_type& i,
        const object_type& o,
        sqlite::statement_kind sk)
  {
    ODB_POTENTIALLY_UNUSED (i);
    ODB_POTENTIALLY_UNUSED (o);
    ODB_POTENTIALLY_UNUSED (sk);

    using namespace sqlite;

    bool grew (false);

    // job_id
    //
    if (sk == statement_insert)
    {
      unsigned int const& v =
        o.job_id;

      bool is_null (false);
      sqlite::value_traits<
          unsigned int,
          sqlite::id_integer >::set_image (
        i.job_id_value,
        is_null,
        v);
      i.job_id_null = is_null;
    }

    // sha256
    //
    {
      ::std::string const& v =
        o.sha256;

      bool is_null (false);
      std::size_t cap (i.sha256_value.capacity ());
      sqlite::value_traits<
          ::std::string,
          sqlite::id_text >::set_image (
        i.sha256_value,
        i.sha256_size,
        is_null,
        v);
      i.sha256_null = is_null;
      grew = grew || (cap != i.sha256_value.capacity ());
    }

    // imageRecord
    //
    {
      ::imageHasher::db::table::ImageRecord* const& v =
        o.imageRecord;

      typedef object_traits< ::imageHasher::db::table::ImageRecord > obj_traits;
      typedef odb::pointer_traits< ::imageHasher::db::table::ImageRecord* > ptr_traits;

      bool is_null (ptr_traits::null_ptr (v));
      if (!is_null)
      {
        const obj_traits::id_type& id (
          obj_traits::id (ptr_traits::get_ref (v)));

        sqlite::value_traits<
            obj_traits::id_type,
            sqlite::id_integer >::set_image (
          i.imageRecord_value,
          is_null,
          id);
        i.imageRecord_null = is_null;
      }
      else
        i.imageRecord_null = true;
    }

    return grew;
  }

  void access::object_traits_impl< ::imageHasher::db::table::HashJob, id_sqlite >::
  init (object_type& o,
        const image_type& i,
        database* db)
  {
    ODB_POTENTIALLY_UNUSED (o);
    ODB_POTENTIALLY_UNUSED (i);
    ODB_POTENTIALLY_UNUSED (db);

    // job_id
    //
    {
      unsigned int& v =
        o.job_id;

      sqlite::value_traits<
          unsigned int,
          sqlite::id_integer >::set_value (
        v,
        i.job_id_value,
        i.job_id_null);
    }

    // sha256
    //
    {
      ::std::string& v =
        o.sha256;

      sqlite::value_traits<
          ::std::string,
          sqlite::id_text >::set_value (
        v,
        i.sha256_value,
        i.sha256_size,
        i.sha256_null);
    }

    // imageRecord
    //
    {
      ::imageHasher::db::table::ImageRecord*& v =
        o.imageRecord;

      typedef object_traits< ::imageHasher::db::table::ImageRecord > obj_traits;
      typedef odb::pointer_traits< ::imageHasher::db::table::ImageRecord* > ptr_traits;

      if (i.imageRecord_null)
        v = ptr_traits::pointer_type ();
      else
      {
        obj_traits::id_type id;
        sqlite::value_traits<
            obj_traits::id_type,
            sqlite::id_integer >::set_value (
          id,
          i.imageRecord_value,
          i.imageRecord_null);

        // If a compiler error points to the line below, then
        // it most likely means that a pointer used in a member
        // cannot be initialized from an object pointer.
        //
        v = ptr_traits::pointer_type (
          static_cast<sqlite::database*> (db)->load<
            obj_traits::object_type > (id));
      }
    }
  }

  void access::object_traits_impl< ::imageHasher::db::table::HashJob, id_sqlite >::
  init (id_image_type& i, const id_type& id)
  {
    {
      bool is_null (false);
      sqlite::value_traits<
          unsigned int,
          sqlite::id_integer >::set_image (
        i.id_value,
        is_null,
        id);
      i.id_null = is_null;
    }
  }

  const char access::object_traits_impl< ::imageHasher::db::table::HashJob, id_sqlite >::persist_statement[] =
  "INSERT INTO \"hashjob\" "
  "(\"job_id\", "
  "\"sha256\", "
  "\"imageRecord\") "
  "VALUES "
  "(?, ?, ?)";

  const char access::object_traits_impl< ::imageHasher::db::table::HashJob, id_sqlite >::find_statement[] =
  "SELECT "
  "\"hashjob\".\"job_id\", "
  "\"hashjob\".\"sha256\", "
  "\"hashjob\".\"imageRecord\" "
  "FROM \"hashjob\" "
  "WHERE \"hashjob\".\"job_id\"=?";

  const char access::object_traits_impl< ::imageHasher::db::table::HashJob, id_sqlite >::update_statement[] =
  "UPDATE \"hashjob\" "
  "SET "
  "\"sha256\"=?, "
  "\"imageRecord\"=? "
  "WHERE \"job_id\"=?";

  const char access::object_traits_impl< ::imageHasher::db::table::HashJob, id_sqlite >::erase_statement[] =
  "DELETE FROM \"hashjob\" "
  "WHERE \"job_id\"=?";

  const char access::object_traits_impl< ::imageHasher::db::table::HashJob, id_sqlite >::query_statement[] =
  "SELECT\n"
  "\"hashjob\".\"job_id\",\n"
  "\"hashjob\".\"sha256\",\n"
  "\"hashjob\".\"imageRecord\"\n"
  "FROM \"hashjob\"\n"
  "LEFT JOIN \"imagerecord\" AS \"imageRecord\" ON \"imageRecord\".\"image_id\"=\"hashjob\".\"imageRecord\"";

  const char access::object_traits_impl< ::imageHasher::db::table::HashJob, id_sqlite >::erase_query_statement[] =
  "DELETE FROM \"hashjob\"";

  const char access::object_traits_impl< ::imageHasher::db::table::HashJob, id_sqlite >::table_name[] =
  "\"hashjob\"";

  void access::object_traits_impl< ::imageHasher::db::table::HashJob, id_sqlite >::
  persist (database& db, object_type& obj)
  {
    ODB_POTENTIALLY_UNUSED (db);

    using namespace sqlite;

    sqlite::connection& conn (
      sqlite::transaction::current ().connection ());
    statements_type& sts (
      conn.statement_cache ().find_object<object_type> ());

    callback (db,
              static_cast<const object_type&> (obj),
              callback_event::pre_persist);

    image_type& im (sts.image ());
    binding& imb (sts.insert_image_binding ());

    if (init (im, obj, statement_insert))
      im.version++;

    im.job_id_null = true;

    if (im.version != sts.insert_image_version () ||
        imb.version == 0)
    {
      bind (imb.bind, im, statement_insert);
      sts.insert_image_version (im.version);
      imb.version++;
    }

    insert_statement& st (sts.persist_statement ());
    if (!st.execute ())
      throw object_already_persistent ();

    obj.job_id = static_cast< id_type > (st.id ());

    callback (db,
              static_cast<const object_type&> (obj),
              callback_event::post_persist);
  }

  void access::object_traits_impl< ::imageHasher::db::table::HashJob, id_sqlite >::
  update (database& db, const object_type& obj)
  {
    ODB_POTENTIALLY_UNUSED (db);

    using namespace sqlite;
    using sqlite::update_statement;

    callback (db, obj, callback_event::pre_update);

    sqlite::transaction& tr (sqlite::transaction::current ());
    sqlite::connection& conn (tr.connection ());
    statements_type& sts (
      conn.statement_cache ().find_object<object_type> ());

    const id_type& id (
      obj.job_id);
    id_image_type& idi (sts.id_image ());
    init (idi, id);

    image_type& im (sts.image ());
    if (init (im, obj, statement_update))
      im.version++;

    bool u (false);
    binding& imb (sts.update_image_binding ());
    if (im.version != sts.update_image_version () ||
        imb.version == 0)
    {
      bind (imb.bind, im, statement_update);
      sts.update_image_version (im.version);
      imb.version++;
      u = true;
    }

    binding& idb (sts.id_image_binding ());
    if (idi.version != sts.update_id_image_version () ||
        idb.version == 0)
    {
      if (idi.version != sts.id_image_version () ||
          idb.version == 0)
      {
        bind (idb.bind, idi);
        sts.id_image_version (idi.version);
        idb.version++;
      }

      sts.update_id_image_version (idi.version);

      if (!u)
        imb.version++;
    }

    update_statement& st (sts.update_statement ());
    if (st.execute () == 0)
      throw object_not_persistent ();

    callback (db, obj, callback_event::post_update);
    pointer_cache_traits::update (db, obj);
  }

  void access::object_traits_impl< ::imageHasher::db::table::HashJob, id_sqlite >::
  erase (database& db, const id_type& id)
  {
    using namespace sqlite;

    ODB_POTENTIALLY_UNUSED (db);

    sqlite::connection& conn (
      sqlite::transaction::current ().connection ());
    statements_type& sts (
      conn.statement_cache ().find_object<object_type> ());

    id_image_type& i (sts.id_image ());
    init (i, id);

    binding& idb (sts.id_image_binding ());
    if (i.version != sts.id_image_version () || idb.version == 0)
    {
      bind (idb.bind, i);
      sts.id_image_version (i.version);
      idb.version++;
    }

    if (sts.erase_statement ().execute () != 1)
      throw object_not_persistent ();

    pointer_cache_traits::erase (db, id);
  }

  access::object_traits_impl< ::imageHasher::db::table::HashJob, id_sqlite >::pointer_type
  access::object_traits_impl< ::imageHasher::db::table::HashJob, id_sqlite >::
  find (database& db, const id_type& id)
  {
    using namespace sqlite;

    {
      pointer_type p (pointer_cache_traits::find (db, id));

      if (!pointer_traits::null_ptr (p))
        return p;
    }

    sqlite::connection& conn (
      sqlite::transaction::current ().connection ());
    statements_type& sts (
      conn.statement_cache ().find_object<object_type> ());

    statements_type::auto_lock l (sts);

    if (l.locked ())
    {
      if (!find_ (sts, &id))
        return pointer_type ();
    }

    pointer_type p (
      access::object_factory<object_type, pointer_type>::create ());
    pointer_traits::guard pg (p);

    pointer_cache_traits::insert_guard ig (
      pointer_cache_traits::insert (db, id, p));

    object_type& obj (pointer_traits::get_ref (p));

    if (l.locked ())
    {
      select_statement& st (sts.find_statement ());
      ODB_POTENTIALLY_UNUSED (st);

      callback (db, obj, callback_event::pre_load);
      init (obj, sts.image (), &db);
      load_ (sts, obj, false);
      sts.load_delayed (0);
      l.unlock ();
      callback (db, obj, callback_event::post_load);
      pointer_cache_traits::load (ig.position ());
    }
    else
      sts.delay_load (id, obj, ig.position ());

    ig.release ();
    pg.release ();
    return p;
  }

  bool access::object_traits_impl< ::imageHasher::db::table::HashJob, id_sqlite >::
  find (database& db, const id_type& id, object_type& obj)
  {
    using namespace sqlite;

    sqlite::connection& conn (
      sqlite::transaction::current ().connection ());
    statements_type& sts (
      conn.statement_cache ().find_object<object_type> ());

    statements_type::auto_lock l (sts);

    if (!find_ (sts, &id))
      return false;

    select_statement& st (sts.find_statement ());
    ODB_POTENTIALLY_UNUSED (st);

    reference_cache_traits::position_type pos (
      reference_cache_traits::insert (db, id, obj));
    reference_cache_traits::insert_guard ig (pos);

    callback (db, obj, callback_event::pre_load);
    init (obj, sts.image (), &db);
    load_ (sts, obj, false);
    sts.load_delayed (0);
    l.unlock ();
    callback (db, obj, callback_event::post_load);
    reference_cache_traits::load (pos);
    ig.release ();
    return true;
  }

  bool access::object_traits_impl< ::imageHasher::db::table::HashJob, id_sqlite >::
  reload (database& db, object_type& obj)
  {
    using namespace sqlite;

    sqlite::connection& conn (
      sqlite::transaction::current ().connection ());
    statements_type& sts (
      conn.statement_cache ().find_object<object_type> ());

    statements_type::auto_lock l (sts);

    const id_type& id  (
      obj.job_id);

    if (!find_ (sts, &id))
      return false;

    select_statement& st (sts.find_statement ());
    ODB_POTENTIALLY_UNUSED (st);

    callback (db, obj, callback_event::pre_load);
    init (obj, sts.image (), &db);
    load_ (sts, obj, true);
    sts.load_delayed (0);
    l.unlock ();
    callback (db, obj, callback_event::post_load);
    return true;
  }

  bool access::object_traits_impl< ::imageHasher::db::table::HashJob, id_sqlite >::
  find_ (statements_type& sts,
         const id_type* id)
  {
    using namespace sqlite;

    id_image_type& i (sts.id_image ());
    init (i, *id);

    binding& idb (sts.id_image_binding ());
    if (i.version != sts.id_image_version () || idb.version == 0)
    {
      bind (idb.bind, i);
      sts.id_image_version (i.version);
      idb.version++;
    }

    image_type& im (sts.image ());
    binding& imb (sts.select_image_binding ());

    if (im.version != sts.select_image_version () ||
        imb.version == 0)
    {
      bind (imb.bind, im, statement_select);
      sts.select_image_version (im.version);
      imb.version++;
    }

    select_statement& st (sts.find_statement ());

    st.execute ();
    auto_result ar (st);
    select_statement::result r (st.fetch ());

    if (r == select_statement::truncated)
    {
      if (grow (im, sts.select_image_truncated ()))
        im.version++;

      if (im.version != sts.select_image_version ())
      {
        bind (imb.bind, im, statement_select);
        sts.select_image_version (im.version);
        imb.version++;
        st.refetch ();
      }
    }

    return r != select_statement::no_data;
  }

  result< access::object_traits_impl< ::imageHasher::db::table::HashJob, id_sqlite >::object_type >
  access::object_traits_impl< ::imageHasher::db::table::HashJob, id_sqlite >::
  query (database&, const query_base_type& q)
  {
    using namespace sqlite;
    using odb::details::shared;
    using odb::details::shared_ptr;

    sqlite::connection& conn (
      sqlite::transaction::current ().connection ());

    statements_type& sts (
      conn.statement_cache ().find_object<object_type> ());

    image_type& im (sts.image ());
    binding& imb (sts.select_image_binding ());

    if (im.version != sts.select_image_version () ||
        imb.version == 0)
    {
      bind (imb.bind, im, statement_select);
      sts.select_image_version (im.version);
      imb.version++;
    }

    std::string text (query_statement);
    if (!q.empty ())
    {
      text += "\n";
      text += q.clause ();
    }

    q.init_parameters ();
    shared_ptr<select_statement> st (
      new (shared) select_statement (
        conn,
        text,
        true,
        true,
        q.parameters_binding (),
        imb));

    st->execute ();

    shared_ptr< odb::object_result_impl<object_type> > r (
      new (shared) sqlite::object_result_impl<object_type> (
        q, st, sts, 0));

    return result<object_type> (r);
  }

  unsigned long long access::object_traits_impl< ::imageHasher::db::table::HashJob, id_sqlite >::
  erase_query (database&, const query_base_type& q)
  {
    using namespace sqlite;

    sqlite::connection& conn (
      sqlite::transaction::current ().connection ());

    std::string text (erase_query_statement);
    if (!q.empty ())
    {
      text += ' ';
      text += q.clause ();
    }

    q.init_parameters ();
    delete_statement st (
      conn,
      text,
      q.parameters_binding ());

    return st.execute ();
  }

  odb::details::shared_ptr<prepared_query_impl>
  access::object_traits_impl< ::imageHasher::db::table::HashJob, id_sqlite >::
  prepare_query (connection& c, const char* n, const query_base_type& q)
  {
    using namespace sqlite;
    using odb::details::shared;
    using odb::details::shared_ptr;

    sqlite::connection& conn (
      static_cast<sqlite::connection&> (c));

    statements_type& sts (
      conn.statement_cache ().find_object<object_type> ());

    image_type& im (sts.image ());
    binding& imb (sts.select_image_binding ());

    if (im.version != sts.select_image_version () ||
        imb.version == 0)
    {
      bind (imb.bind, im, statement_select);
      sts.select_image_version (im.version);
      imb.version++;
    }

    std::string text (query_statement);
    if (!q.empty ())
    {
      text += "\n";
      text += q.clause ();
    }

    shared_ptr<sqlite::prepared_query_impl> r (
      new (shared) sqlite::prepared_query_impl (conn));
    r->name = n;
    r->execute = &execute_query;
    r->query = q;
    r->stmt.reset (
      new (shared) select_statement (
        conn,
        text,
        true,
        true,
        r->query.parameters_binding (),
        imb));

    return r;
  }

  odb::details::shared_ptr<result_impl>
  access::object_traits_impl< ::imageHasher::db::table::HashJob, id_sqlite >::
  execute_query (prepared_query_impl& q)
  {
    using namespace sqlite;
    using odb::details::shared;
    using odb::details::shared_ptr;

    sqlite::prepared_query_impl& pq (
      static_cast<sqlite::prepared_query_impl&> (q));
    shared_ptr<select_statement> st (
      odb::details::inc_ref (
        static_cast<select_statement*> (pq.stmt.get ())));

    sqlite::connection& conn (
      sqlite::transaction::current ().connection ());

    // The connection used by the current transaction and the
    // one used to prepare this statement must be the same.
    //
    assert (&conn == &st->connection ());

    statements_type& sts (
      conn.statement_cache ().find_object<object_type> ());

    image_type& im (sts.image ());
    binding& imb (sts.select_image_binding ());

    if (im.version != sts.select_image_version () ||
        imb.version == 0)
    {
      bind (imb.bind, im, statement_select);
      sts.select_image_version (im.version);
      imb.version++;
    }

    pq.query.init_parameters ();
    st->execute ();

    return shared_ptr<result_impl> (
      new (shared) sqlite::object_result_impl<object_type> (
        pq.query, st, sts, 0));
  }
}

namespace odb
{
  static bool
  create_schema (database& db, unsigned short pass, bool drop)
  {
    ODB_POTENTIALLY_UNUSED (db);
    ODB_POTENTIALLY_UNUSED (pass);
    ODB_POTENTIALLY_UNUSED (drop);

    if (drop)
    {
      switch (pass)
      {
        case 1:
        {
          return true;
        }
        case 2:
        {
          db.execute ("DROP TABLE IF EXISTS \"hashjob\"");
          return false;
        }
      }
    }
    else
    {
      switch (pass)
      {
        case 1:
        {
          db.execute ("CREATE TABLE \"hashjob\" (\n"
                      "  \"job_id\" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\n"
                      "  \"sha256\" TEXT NOT NULL,\n"
                      "  \"imageRecord\" INTEGER NULL,\n"
                      "  CONSTRAINT \"imageRecord_fk\"\n"
                      "    FOREIGN KEY (\"imageRecord\")\n"
                      "    REFERENCES \"imagerecord\" (\"image_id\")\n"
                      "    DEFERRABLE INITIALLY DEFERRED)");
          db.execute ("CREATE INDEX \"hashjob_sha256_i\"\n"
                      "  ON \"hashjob\" (\"sha256\")");
          db.execute ("CREATE UNIQUE INDEX \"hashjob_imageRecord_i\"\n"
                      "  ON \"hashjob\" (\"imageRecord\")");
          return false;
        }
      }
    }

    return false;
  }

  static const schema_catalog_create_entry
  create_schema_entry_ (
    id_sqlite,
    "",
    &create_schema);
}

#include <odb/post.hxx>
