#pragma once

#if GPUINSTANCING
   static uint venus_InstanceID;
   void SetupInstanceID(uint instanceID)
   {
      venus_InstanceID = instanceID;
   }

   #define INSTANCED_ARRAY_SIZE 100
   #define VERTEX_INPUT_INSTANCE_ID uint instanceID : SV_InstanceID;
   #define GET_INSTANCE_ID(input) input.instanceID
   #define SETUP_INSTANCE_ID(input) SetupInstanceID(GET_INSTANCE_ID(input))

   #define INSTANCING_BUFFER_START(arr)  struct arr##struct {
   #define INSTANCING_BUFFER_END(arr) }; \
				cbuffer arr { \
				arr##struct arr##Array[INSTANCED_ARRAY_SIZE]; }
   #define DEFINE_INSTANCED_PROP(type, var) type var;
   #define ACCESS_INSTANCED_PROP(arr, var)  arr##Array[venus_InstanceID].var

   #undef LOCALWORLD_TRANSFORM

   INSTANCING_BUFFER_START(INSTANCE_CBUFFER0)
       DEFINE_INSTANCED_PROP(float4x4, LOCALWORLD_TRANSFORM)
   INSTANCING_BUFFER_END(INSTANCE_CBUFFER0)
   
   //
   INSTANCING_BUFFER_START(INSTANCE_CBUFFER1)
       DEFINE_INSTANCED_PROP(float4x4, WorldToObjectArray)
   INSTANCING_BUFFER_END(INSTANCE_CBUFFER1)
   
   #define ObjToWorld ACCESS_INSTANCED_PROP(INSTANCE_CBUFFER0, LOCALWORLD_TRANSFORM)
#else
   #define VERTEX_INPUT_INSTANCE_ID
   #define SETUP_INSTANCE_ID(input)
   #define INSTANCING_BUFFER_START(name)
   #define INSTANCING_BUFFER_END(arr)
   #define DEFINE_INSTANCED_PROP(type, var) type var;
   #define ACCESS_INSTANCED_PROP(arr, var) var
   #define ObjToWorld LOCALWORLD_TRANSFORM
#endif
