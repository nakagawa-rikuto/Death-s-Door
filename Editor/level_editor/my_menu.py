import bpy

from .stretch_vertex import MYADDON_OT_stretch_vertex
from .create_ico_sphere import MYADDON_OT_create_ico_sphere
from .export_scene import MYADDON_OT_export_scene

# *********************************************************************************
# トップバーの拡張メニュー
# *********************************************************************************
class TOPBAR_MT_my_menu(bpy.types.Menu):
    # Blenderがクラスを識別する為の固有の文字列
    bl_idname = "TOPBAR_MT_my_menu"
    # メニューのラベルとして表示される文字列
    bl_label = "MyMenu"
    # 説明表示用の文字列
    bl_description = "拡張メニュー by " + __name__

   # サブメニューの描画
    def draw(self, context):
        self.layout.operator("wm.url_open_preset", text="Manual", icon='HELP')
        self.layout.operator("wm.url_open_preset", text="Manual", icon='HELP')
    
        # 区切り線
        self.layout.separator()
        self.layout.operator("wm.url_open_preset", text="Manual", icon='HELP')

        # 区切り線
        self.layout.separator()
        self.layout.operator("wm.url_open_preset", text="Manual", icon='HELP')

        # トップバーの「エディターメニュー」に項目（オペレータ）を追加
        # 頂点を伸ばす
        self.layout.operator(MYADDON_OT_stretch_vertex.bl_idname,
                             text=MYADDON_OT_stretch_vertex.bl_label)
        
        # ICO球を生成
        self.layout.operator(MYADDON_OT_create_ico_sphere.bl_idname,
                             text=MYADDON_OT_create_ico_sphere.bl_label)
        
        # # シーン出力
        self.layout.operator(MYADDON_OT_export_scene.bl_idname,
                             text=MYADDON_OT_export_scene.bl_label)

    # 既存のメニューにサブメニューを追加
    def submenu(self, context):
        # ID指定でサブメニューを追加
        self.layout.menu(TOPBAR_MT_my_menu.bl_idname)