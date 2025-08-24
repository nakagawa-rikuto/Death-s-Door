import bpy

from .add_disabled import MYADDON_OT_add_disabled

# *********************************************************************************
# パネル コライダー
# *********************************************************************************
class OBJECT_PT_disabled(bpy.types.Panel):
    """オブジェクトのファイルネームパネル"""
    bl_idname = "OBJECT_PT_disabled"
    bl_label = "disabled"
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "object"

   # サブメニューの描画
    def draw(self, context):
        layout = self.layout

        # パネルに項目を追加
        if "disabled" in context.object:
            # 既にプロパティがあれば、プロパティを表示
            self.layout.prop(context.object, '["disabled"]', text = "True")
        else:
            # プロパティがなければ、プロパティ追加ボタンを表示
            self.layout.operator(MYADDON_OT_add_disabled.bl_idname, text = "disabledを追加")