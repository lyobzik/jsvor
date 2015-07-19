#include "ValidationContext.h"

#include <JsonErrors.h>

#include "Defs.h"

namespace JsonSchemaValidator {

template <typename ElementName>
PathHolder<ElementName>::PathHolder(ElementName const &name, ValidationContext &context)
	: need_set_(true)
	, name_(name)
	, context_(context) {
}

template <typename ElementName>
PathHolder<ElementName>::~PathHolder() {
	if (need_set_) {
		context_.GetResult().AddPath(ElementNameToString());
	}
}

template <typename ElementName>
void PathHolder<ElementName>::Reset() {
	need_set_ = false;
}

} // namespace JsonSchemaValidator
