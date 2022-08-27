#include "kernel.h"
#include "../hylic_ast.h"
#include "../hylic_eval.h"
#include "ffi.h"
#include "net.h"
#include "fs.h"
#include "io.h"
#include "../type_util.h"
#include "../system.h"

#include <iostream>
#include <string>
#include <vector>

std::map<SystemModule, std::map<std::string, AstNode *>> kernel_map;

std::map<std::string, Entity*> system_entities;

// Always 1, because we count the Monad
int n_running_programs = 1;

EntityRefNode* get_entity_ref(Entity* e) {
  return (EntityRefNode*)make_entity_ref(e->address.node_id, e->address.vat_id, e->address.entity_id);
}

void load_system_entity(EvalContext *context, std::string entity_name) {
  auto io_ent = create_entity(context, (EntityDef *)kernel_map[SystemModule::Io]["Io"], false);
  system_entities[entity_name] = io_ent;
}

EntityRefNode* get_system_entity_ref(CType ctype) {
  assert(ctype.basetype == PType::Entity);
  assert(ctype.dtype == DType::Far);

  auto ent = system_entities.find(ctype.entity_name);
  assert(ent != system_entities.end());

  return get_entity_ref(ent->second);
}

AstNode *request_far_entity() {
  return make_number(0);
}

AstNode *monad_start_program(EvalContext *context, std::vector<AstNode*> args) {

  auto eref = (EntityRefNode*)args[0];

  n_running_programs += 1;
  //printf("Incremented programs: %d\n", n_running_programs);
  //printf("Called eref %d %d %d\n", eref->node_id, eref->vat_id, eref->entity_id);
  eval_message_node(context, eref, CommMode::Async, "main", {make_number(0)});

  return make_number(0);
}

AstNode *monad_n_programs(EvalContext *context, std::vector<AstNode *> args) {
  return make_string(std::to_string(n_running_programs));
}

AstNode *monad_create(EvalContext *context, std::vector<AstNode *> args) {
  return make_number(0);
}

AstNode *monad_hello(EvalContext *context, std::vector<AstNode *> args) {
  load_system_entity(context, "Io");

  auto eref = get_entity_ref(system_entities["Io"]);

  //eval_message_node(context, eref, CommMode::Sync, "print", {make_string("hi")});
  return make_number(0);
}

void load_kernel() {

  std::map<std::string, FuncStmt *> functions;

  CType *c = new CType;
  *c = lu8();

  functions["hello"] = setup_direct_call(monad_hello, "hello", {"i"}, {c}, lu8());
  functions["create"] = setup_direct_call(monad_create, "create", {}, {}, lu8());

  CType *c2 = new CType;
  c2->basetype = PType::BaseEntity;
  c2->dtype = DType::Far;

  functions["start-program"] = setup_direct_call(monad_start_program, "start-program", {"eref"}, {c2}, lu8());
  functions["n-programs"] = setup_direct_call(monad_n_programs, "n-programs", {}, {}, lstr());

  kernel_map[SystemModule::Monad] = {
    {"Monad", make_actor(nullptr, "Monad", functions, {}, {})}
  };

  kernel_map[SystemModule::Io] = load_io();
  kernel_map[SystemModule::Net] = load_net();
  load_fs();
  //load_amoeba();
}
