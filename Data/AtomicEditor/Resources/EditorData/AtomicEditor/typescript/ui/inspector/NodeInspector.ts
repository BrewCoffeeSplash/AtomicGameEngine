
import ScriptWidget = require("../ScriptWidget");

var UI = Atomic.UI;

class NodeInspector extends ScriptWidget {

  constructor() {

    super();

  }

  inspect(node:Atomic.Node) {

    var fd = new Atomic.UIFontDescription();
    fd.id = "Vera";
    fd.size = 11;

    var nlp = new Atomic.UILayoutParams();
    nlp.width = 304;

    var nodeLayout = new Atomic.UILayout();
    nodeLayout.spacing = 4;

    nodeLayout.layoutDistribution  = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;
    nodeLayout.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
    nodeLayout.layoutParams = nlp;

    var nodeContainer = new Atomic.UIContainer();
    nodeContainer.gravity = Atomic.UI_GRAVITY_ALL;
    nodeContainer.skinBg = "InspectorTopLayout";

    // node attr layout

    var attrsVerticalLayout = new Atomic.UILayout(Atomic.UI_AXIS_Y);
    attrsVerticalLayout.gravity = Atomic.UI_GRAVITY_ALL;
    attrsVerticalLayout.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
    nodeContainer.addChild(attrsVerticalLayout);

    var nodeLabel = new Atomic.UITextField();
    nodeLabel.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
    nodeLabel.text = "Node";
    nodeLabel.skinBg = "InspectorTextLabel";

    attrsVerticalLayout.addChild(nodeLabel);

    var attrs = node.getAttributes();

    for (var i in attrs) {

      var attr = attrs[i];

      if (attr.mode & Atomic.AM_NOEDIT)
        continue;

      var attrLayout = new Atomic.UILayout();

      attrLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;

      var name = new Atomic.UITextField();
      name.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
      name.skinBg = "InspectorTextAttrName";

      var bname = attr.name;
      if (bname == "Is Enabled")
          bname = "Enabled";

      name.text = bname;
      name.fontDescription = fd;

      attrLayout.addChild(name);

      //TBWidget* bwidget = binding->GetWidget();
      //attrLayout->AddChild(bwidget);

      attrsVerticalLayout.addChild(attrLayout);



    }

    nodeLayout.addChild(nodeContainer);

    this.addChild(nodeLayout);

  }


}

export = NodeInspector;
