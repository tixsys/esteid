package ee.sk.digidoc.c14n;

import ee.sk.digidoc.c14n.EntityParser_Entity;

public abstract interface EntityParser_Handler
{




    abstract public String ResolveEntity(EntityParser_Entity e);

    abstract public String ResolveText(String e);

}
