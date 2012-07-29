//
//  Shader.fsh
//  Crow-Regime(iOS)
//
//  Created by Jody McAdams on 7/28/12.
//
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
