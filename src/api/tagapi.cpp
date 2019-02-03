#include "tagapi.h"

TagApi* TagApi::fetch(int id) {
    Tag tag = Tag::fetch(id);

    if (tag.isFetched()) {
        this->id = tag.getId();
        name = tag.getName();
        parentId = tag.getParentId();
        priority = tag.getPriority();
    }

    return this;
}

TagApi* TagApi::fetchByName(const QString &name, int parentId) {
    Tag tag = Tag::fetchByName(name, parentId);

    if (tag.isFetched()) {
        this->id = tag.getId();
        this->name = tag.getName();
        this->parentId = tag.getParentId();
        this->priority = tag.getPriority();
    }

    return this;
}
