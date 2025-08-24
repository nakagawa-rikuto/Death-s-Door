import bpy
import mathutils
import gpu # type: ignore
import gpu_extras.batch # type: ignore
import copy

# *********************************************************************************
# コライダー描画
# *********************************************************************************
class DrawCollider:

    # 描画ハンドル
    handle = None

    # 3Dビューに登録する描画関数
    def draw_collider():
        #頂点データ
        vertices = {"pos": []}
        # インデックスデータ
        indices = []

        # 各頂点の、オブジェクト中心からオフセット
        offsets = [
            [-0.5, -0.5, -0.5],  # 左下前
            [+0.5, -0.5, -0.5],  # 右下前
            [-0.5, +0.5, -0.5],  # 左上前
            [+0.5, +0.5, -0.5],  # 右上前
            [-0.5, -0.5, +0.5],  # 左下奥
            [+0.5, -0.5, +0.5],  # 右下奥
            [-0.5, +0.5, +0.5],  # 左上奥
            [+0.5, +0.5, +0.5],  # 右上奥
        ]

        # 立方体のX, Y, Z方向のサイズ
        size = [2,2,2]

        # シーンのオブジェクトリストを走査
        for object in bpy.context.scene.objects:

            # コライダープロパティがなければ、描画をスキップ
            if not "collider" in object:
                continue

            # 中心点, サイズの変数を宣言
            center = mathutils.Vector((0, 0, 0))
            size = mathutils.Vector((2, 2, 2))

            # プロパティから値を取得
            center[0] = object["collider_center"][0]
            center[1] = object["collider_center"][1]
            center[2] = object["collider_center"][2]
            size[0] = object["collider_size"][0]
            size[1] = object["collider_size"][1]
            size[2] = object["collider_size"][2]
            

            # 追加前の頂点数を記録
            start = len(vertices["pos"])

            # Boxの8頂点を処理
            for offset in offsets:
                # オブジェクトの中心座標をコピー
                pos = copy.copy(center)
                # 各軸方向にサイズとオフセットを適用
                pos[0] += offset[0] * size[0]
                pos[1] += offset[1] * size[1]
                pos[2] += offset[2] * size[2]
                # ローカル座標空ワールド座標に変換
                pos = object.matrix_world @ pos # @は掛け算の演算子
                # 頂点データとして追加
                vertices["pos"].append(pos)

                # 各面を構成する辺のインデックス
                # 前面（Zマイナス側）
                indices.append([start+0, start+1])
                indices.append([start+2, start+3])
                indices.append([start+0, start+2])
                indices.append([start+1, start+3])

                # 背面（Zプラス側）
                indices.append([start+4, start+5])
                indices.append([start+6, start+7])
                indices.append([start+4, start+6])
                indices.append([start+5, start+7])

                # 前後をつなぐ辺
                indices.append([start+0, start+4])
                indices.append([start+1, start+5])
                indices.append([start+2, start+6])
                indices.append([start+3, start+7])

        # ビルトインのシェーダーを取得
        # 3D_UNIFORM_COLORはない
        shader = gpu.shader.from_builtin("UNIFORM_COLOR")

        # バッチを作成（引数 : シェーダー、 トポロジー、頂点データ、　インデックスデータ）
        batch = gpu_extras.batch.batch_for_shader(shader, "LINES", vertices, indices = indices)

        # シェーダーのパラメーター設定
        color = [0.5, 1.0, 1.0, 1.0]
        shader.bind()
        shader.uniform_float("color", color)
        # 描画
        batch.draw(shader)
