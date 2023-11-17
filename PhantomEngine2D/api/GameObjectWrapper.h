#pragma once
#include <string>
#include <mono/jit/jit.h>
#include <mono/metadata/metadata.h>
#include <mono/metadata/object.h>

#ifdef __cplusplus
extern "C" {
#endif

	struct GameObject;

	// Functions for C# to interact with GameObject
	std::string GetName(MonoObject* gameObject);
	void SetName(MonoObject* gameObject, MonoString* name);
	MonoObject* GetPosition(MonoObject* gameObject);
	void SetPositon(MonoObject* gameObject, MonoObject* position);
	MonoObject* GetSize(MonoObject* gameObject);
	void SetSize(MonoObject* gameObject, MonoObject* size);
	MonoObject* GetRotation(MonoObject* gameObject);
	void SetRotation(MonoObject* gameObject, MonoObject* rotation);

#ifdef __cplusplus
}
#endif
