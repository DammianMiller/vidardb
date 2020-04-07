//  Copyright (c) 2019-present, VidarDB, Inc.  All rights reserved.
//  This source code is licensed under the BSD-style license found in the
//  LICENSE file in the root directory of this source tree. An additional grant
//  of patent rights can be found in the PATENTS file in the same directory.

#include <iostream>
using namespace std;

#include "vidardb/db.h"
#include "vidardb/status.h"
#include "vidardb/options.h"
#include "vidardb/splitter.h"
#include "vidardb/table.h"
using namespace vidardb;

unsigned int M = 3;
string kDBPath = "/tmp/vidardb_range_query_column_example";

int main(int argc, char* argv[]) {
  // remove existed db path
  int ret = system(string("rm -rf " + kDBPath).c_str());

  // open database
  DB* db;
  Options options;
  options.create_if_missing = true;

  const Splitter* splitter = NewEncodingSplitter();
  options.splitter = splitter;

  // column table
  TableFactory* table_factory = NewColumnTableFactory();
  ColumnTableOptions* opts =
      static_cast<ColumnTableOptions*>(table_factory->GetOptions());
  opts->column_count = M;
  options.table_factory.reset(table_factory);

  Status s = DB::Open(options, kDBPath, &db);
  assert(s.ok());

  // insert data
  WriteOptions write_options;
  // write_options.sync = true;
  s = db->Put(write_options, "1", 
      splitter->Stitch(vector<string>{"chen1", "33", "hangzhou"}));
  assert(s.ok());
  s = db->Put(write_options, "2", 
      splitter->Stitch(vector<string>{"wang2", "32", "wuhan"}));
  assert(s.ok());
  s = db->Put(write_options, "3", 
      splitter->Stitch(vector<string>{"zhao3", "35", "nanjing"}));
  assert(s.ok());
  s = db->Put(write_options, "4", 
      splitter->Stitch(vector<string>{"liao4", "28", "beijing"}));
  assert(s.ok());
  s = db->Put(write_options, "5", 
      splitter->Stitch(vector<string>{"jiang5", "30", "shanghai"}));
  assert(s.ok());
  s = db->Put(write_options, "6", 
      splitter->Stitch(vector<string>{"lian6", "30", "changsha"}));
  assert(s.ok());
  s = db->Delete(write_options, "1");
  assert(s.ok());
  s = db->Put(write_options, "3", 
      splitter->Stitch(vector<string>{"zhao333", "35", "nanjing"}));
  assert(s.ok());
  s = db->Put(write_options, "6", 
      splitter->Stitch(vector<string>{"lian666", "30", "changsha"}));
  assert(s.ok());
  s = db->Put(write_options, "1", 
      splitter->Stitch(vector<string>{"chen1111", "33", "hangzhou"}));
  assert(s.ok());
  s = db->Delete(write_options, "3");
  assert(s.ok());

  // test column sstable or memtable
  s = db->Flush(FlushOptions());
  assert(s.ok());

  ReadOptions read_options;
  // read_options.batch_capacity = 0; // full search
  read_options.batch_capacity = 2; // in batch
  // read_options.columns = {0}; // only query keys
  read_options.columns = {1, 3};

  // Range range; // full search
  // Range range("2", "5"); // [2, 5]
  Range range("1", "6"); // [1, 6]
  // Range range("1", kRangeQueryMax); // [1, max]

  list<RangeQueryKeyVal> res;
  bool next = true;
  while (next) { // range query loop
    size_t total_key_size = 0, total_val_size = 0;
    next = db->RangeQuery(read_options, range, res, &s);
    assert(s.ok());
    cout << "{ ";
    for (auto it : res) {
      total_key_size += it.user_key.size();
      total_val_size += it.user_val.size();
      cout << it.user_key << "=[";
      vector<string> vals(splitter->Split(it.user_val));
      for (auto i = 0u; i < vals.size(); i++) {
        cout << vals[i];
        if (i < vals.size() - 1) {
          cout << ", ";
        };
      }
      cout << "] ";
    }
    cout << "} key_size=" << read_options.result_key_size;
    cout << ", val_size=" << read_options.result_val_size << endl;
    assert(total_key_size == read_options.result_key_size);
    assert(total_val_size == read_options.result_val_size);
  }

  delete db, splitter;
  return 0;
}
