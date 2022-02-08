#include "indexed_row_table.h"
#include <cstring>
#include <algorithm>

namespace bytedance_db_project {
IndexedRowTable::IndexedRowTable(int32_t index_column) {
  index_column_ = index_column;
}

void IndexedRowTable::Load(BaseDataLoader *loader) {
  // TODO: Implement this!
  int32_t field;
  num_cols_ = loader->GetNumCols();
  auto rows = loader->GetRows();
  num_rows_ = rows.size();
  rows_ = new char[FIXED_FIELD_LEN * num_rows_ * num_cols_];
  for (auto row_id = 0; row_id < num_rows_; row_id++) {
    auto cur_row = rows.at(row_id);
    std::memcpy(rows_ + row_id * (FIXED_FIELD_LEN * num_cols_), cur_row, FIXED_FIELD_LEN * num_cols_);
    // 存储索引信息
    std::memcpy(&field, rows_ + FIXED_FIELD_LEN * (row_id * num_cols_ + index_column_), FIXED_FIELD_LEN);
    if(index_.find(field) != index_.end()){
      index_[field].push_back(row_id);
    }
    else{
      index_[field] = std::vector<int32_t>(); // insert
      index_[field].push_back(row_id);
    }
  }
}

int32_t IndexedRowTable::GetIntField(int32_t row_id, int32_t col_id) {
  // TODO: Implement this!
  int32_t field;
  memcpy(&field, rows_ + FIXED_FIELD_LEN * (row_id * num_cols_ + col_id), FIXED_FIELD_LEN);
  return field;
}

void IndexedRowTable::PutIntField(int32_t row_id, int32_t col_id,
                                  int32_t field) {
  // TODO: Implement this!
  if(index_column_ == col_id){
    int32_t ori_field;
    memcpy(&ori_field, rows_ + FIXED_FIELD_LEN * (row_id * num_cols_ + col_id), FIXED_FIELD_LEN);
    if(ori_field == field){
      return;
    }
    // 删除index_[ori_field]中对应的row_id，若删除后index_[ori_field]为空, 则删除ori_field对应的pair
    auto iter = find(index_[ori_field].begin(), index_[ori_field].end(), row_id);
    index_[ori_field].erase(iter);
    if(index_[ori_field].empty()){
      index_.erase(ori_field);
    }
  }
  memcpy(rows_ + FIXED_FIELD_LEN * (row_id * num_cols_ + col_id), &field, FIXED_FIELD_LEN);
  // 如果map中没有field对应的pair。则插入新的pair并在index_[field]中插入row_id，否则直接在对应的pair中插入row_id
  if(index_column_ == col_id){  
    if(index_.find(field) != index_.end()){
      index_[field].push_back(row_id);
    }
    else{
      index_[field] = std::vector<int32_t>(); // insert
      index_[field].push_back(row_id);
    }
  }
}

int64_t IndexedRowTable::ColumnSum() {
  // TODO: Implement this!
  int64_t sum = 0;
  if(index_column_ == 0){
    for(auto iter = index_.begin(); iter != index_.end(); iter++){
      sum += (iter->first * iter->second.size());
    }
  }
  else{
    int32_t field;
    for(int32_t row_id = 0; row_id < num_rows_; row_id++){
      memcpy(&field, rows_ + FIXED_FIELD_LEN * (row_id * num_cols_), FIXED_FIELD_LEN);
      sum += field;
    }
  }
  return sum;
}

int64_t IndexedRowTable::PredicatedColumnSum(int32_t threshold1,
                                             int32_t threshold2) {
  // TODO: Implement this!
  int64_t sum = 0;
  int32_t field;
  for(int32_t row_id = 0; row_id < num_rows_; row_id++){
    memcpy(&field, rows_ + FIXED_FIELD_LEN * (row_id * num_cols_ + 1), FIXED_FIELD_LEN);  // col1
    if(field > threshold1){
      memcpy(&field, rows_ + FIXED_FIELD_LEN * (row_id * num_cols_ + 2), FIXED_FIELD_LEN); // col2
      if(field < threshold2){
        memcpy(&field, rows_ + FIXED_FIELD_LEN * (row_id * num_cols_), FIXED_FIELD_LEN); // col0
        sum += field;
      }
    }
  }
  return sum;
}

int64_t IndexedRowTable::PredicatedAllColumnsSum(int32_t threshold) {
  // TODO: Implement this!
  int64_t sum = 0;
  int32_t field;
  for(int32_t row_id=0; row_id<num_rows_; row_id++){
    memcpy(&field, rows_ + FIXED_FIELD_LEN * (row_id * num_cols_), FIXED_FIELD_LEN);    
    if(field > threshold){
      sum += field;
      for(int32_t col_id=1; col_id<num_cols_; col_id++){
        memcpy(&field, rows_ + FIXED_FIELD_LEN * (row_id * num_cols_ + col_id), FIXED_FIELD_LEN);
        sum += field;
      }
    }
  }
  return sum;
}

int64_t IndexedRowTable::PredicatedUpdate(int32_t threshold) {
  // TODO: Implement this!
  int64_t count = 0;
  int32_t ori_field3, field0, field2, field3;
  for(int32_t row_id=0; row_id<num_rows_; row_id++){
    memcpy(&field0, rows_ + FIXED_FIELD_LEN * (row_id * num_cols_), FIXED_FIELD_LEN);
    if(field0 < threshold){
      count++;
      memcpy(&field2, rows_ + FIXED_FIELD_LEN * (row_id * num_cols_ + 2), FIXED_FIELD_LEN);
      if(field2 == 0){
        continue;
      }
      memcpy(&field3, rows_ + FIXED_FIELD_LEN * (row_id * num_cols_ + 3), FIXED_FIELD_LEN);
      ori_field3 = field3;
      field3 += field2;
      if(index_column_ == 3){ // delete
        auto iter = find(index_[ori_field3].begin(), index_[ori_field3].end(), row_id);
        index_[ori_field3].erase(iter);
        if(index_[ori_field3].empty()){
          index_.erase(ori_field3);
        }
      }
      memcpy(rows_ + FIXED_FIELD_LEN * (row_id * num_cols_ + 3), &field3, FIXED_FIELD_LEN);
      if(index_column_ == 3){  // insert
        if(index_.find(field3) != index_.end()){
          index_[field3].push_back(row_id);
        }
        else{
          index_[field3] = std::vector<int32_t>(); // insert
          index_[field3].push_back(row_id);
        }
      }
    }
  }
  return count;
}
} // namespace bytedance_db_project