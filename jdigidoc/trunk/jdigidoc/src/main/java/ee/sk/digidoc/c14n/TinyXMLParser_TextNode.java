package ee.sk.digidoc.c14n;

import ee.sk.digidoc.c14n.TinyXMLParser_Fragment;
import ee.sk.digidoc.c14n.TinyXMLParser_Node;

public final class TinyXMLParser_TextNode extends TinyXMLParser_Node
{
    public TinyXMLParser_Fragment ValueFragment;


    public TinyXMLParser_TextNode()
    {
        super();
    }


    public void ToConsole()
    {
        this.ValueFragment.ToConsole();
    }

}
